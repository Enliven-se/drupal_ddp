<?php
/**
 * @file
 * Drupal DDP re-sync table and functionality.
 */

/**
 * Generate HTML table of unsynced content to Meteor.
 */
function _drupal_ddp_unsynced_content_form(array $form, array &$form_state, $key = 'nid', $type = 'node') {
  if (!isset($form_state['storage']['confirm'])) {
    $table = 'drupal_ddp_sync_' . $type;
    $rows = array();

    $header = array(
      array(
        'data' => strtoupper($key),
        'field' => $key,
      ),
      array(
        'data' => t('Last Synced Attempt'),
        'field' => 'last_attempted_sync',
      ),
      array(
        'data' => t('Synced Successfully?'),
        'field' => 'sync_successful',
      ),
      array(
        'data' => t('Last Successful Sync'),
        'field' => 'last_successful_sync',
      ),
      array(
        'data' => t('@type Title', array('@type' => $type)),
      ),
    );

    // Add extra 'Content Type' row if key is 'nid'.
    if ($key == 'nid') {
      $header[] = array(
        'data' => t('Content Type'),
      );
    }

    $result = db_select($table, 'd')
      ->fields('d')
      ->condition('sync_successful', 0, '=')
      ->extend('TableSort')
      ->orderByHeader($header)
      ->extend('PagerDefault')
      ->limit(50)
      ->execute()
      ->fetchAll();

    foreach ($result as $data) {
      $row = array();
      $row[] = array('data' => $data->$key);
      $row[] = array('data' => format_date($data->last_attempted_sync, $type = 'long'));
      $row[] = array('data' => $data->sync_successful);

      if (empty($data->last_successful_sync)) {
        $row[] = array('data' => $data->last_successful_sync);
      }
      else {
        $row[] = array('data' => format_date($data->last_successful_sync, $type = 'long'));
      }

      if ($key == 'nid') {
        $node = node_load($data->$key);
        $row[] = array('data' => $node->title);
        $row[] = array('data' => $node->type);
      }
      elseif ($key == 'uid') {
        $user = user_load($data->$key);
        $row[] = array('data' => $user->name);
      }
      elseif ($key == 'tid') {
        $term = taxonomy_term_load($data->$key);
        $row[] = array('data' => $term->name);
      }

      $rows[$data->$key] = $row;
    }

    $form['description'] = array(
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('Below is a list of content that unsuccessfully synced
        with Meteor. Select posts and manually resync as needed.'),
    );

    $form['table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $rows,
      '#empty' => t('No records found'),
    );

    $form['pager'] = array(
      '#theme' => 'pager',
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Re-sync content'),
    );
    $form['confirm_clear'] = array(
      '#type' => 'submit',
      '#value' => t('Clear items'),
    );
    return $form;
  }
  else {
    // Grab selected form values to display for confirmation.
    $selected_values = $form_state['values']['table'];
    $selected_values = array_filter($selected_values);

    $message = '';

    foreach ($selected_values as $value) {
      $message .= '<li>' . $value . '</li>';
    }

    $form['info'] = array(
      '#prefix' => '<ul>',
      '#suffix' => '</ul>',
      '#markup' => $message,
    );

    if ($form_state['clicked_button']['#value'] == 'Re-sync content') {
      $button_text = t('Sync');
      $action_text = t('Are you sure you want to Sync these items?');
    }
    else {
      $button_text = t('Clear Queue');
      $action_text = t('Are you sure you want to Clear these items from the Queue?');
    }

    return confirm_form(
      $form,
      $action_text,
      current_path(),
      t('This action cannot be undone.'),
      $button_text,
      t('Cancel')
    );
  }
}

/**
 * Implements hook_form_validate().
 *
 * Validation handler to initiate resync of queued content.
 */
function _drupal_ddp_unsynced_content_form_validate($form, &$form_state) {
  $selected_values = isset($form_state['values']['table']) ? $form_state['values']['table'] : $form_state['storage']['values'];

  // Filter out empty rows from selection.
  $selected_values = array_filter($selected_values);

  if (empty($selected_values)) {
    form_set_error('table', t('No rows selected.'));
  }
}

/**
 * Implements hook_form_submit().
 *
 * Submit handler to initiate resync of queued content.
 */
function _drupal_ddp_unsynced_content_form_submit($form, &$form_state) {
  if (!isset($form_state['storage']['confirm'])) {
    // Add our field to $form_state to keep it around for later.
    $form_state['storage']['values'] = $form_state['values']['table'];

    // This will cause the form to be rebuilt,
    // Entering the confirm part of the form.
    $form_state['storage']['confirm'] = TRUE;
    $form_state['rebuild'] = TRUE;
  }
  else {
    // Get form values from stored location.
    $selected_values = array_filter($form_state['storage']['values']);
    $selected_values = array_values($selected_values);

    // Get form arguments (node, taxonomy, user).
    $args = $form_state['build_info']['args'];

    if ($form_state['values']['op'] == 'Sync') {
      // Reset counter for debug information.
      $_SESSION['http_request_count'] = 0;

      // Execute the function named batch_drupal_ddp_sync_content.
      $batch = drupal_ddp_batch_sync_content($selected_values, $args);
      batch_set($batch);
    }
    elseif ($form_state['values']['op'] == 'Clear Queue') {
      $table = 'drupal_ddp_sync_' . $args[1];

      $rows_deleted = 0;
      foreach ($selected_values as $value) {
        $num_deleted = db_delete($table)
          ->condition($args[0], $value)
          ->execute();

        $rows_deleted = $rows_deleted + $num_deleted;
      }
      drupal_set_message('Cleared ' . $rows_deleted . ' items from Queue without Syncing.');
    }
  }
}

/**
 * Batch Definition: Resave content based on type in args.
 */
function drupal_ddp_batch_sync_content($selected_values, $args) {
  $num_operations = count($selected_values);
  $operations = array();

  for ($i = 0; $i < $num_operations; $i++) {
    $operations[] = array(
      '_drupal_ddp_batch_sync_content_operation',
      array(
        $selected_values[$i],
        $args,
        t('(Operation @operation)', array('@operation' => $i + 1)),
      ),
    );
  }

  $batch = array(
    'operations' => $operations,
    'finished' => 'drupal_ddp_batch_finished',
    'file' => drupal_get_path('module', 'drupal_ddp') . '/drupal_ddp_sync_queue.inc',
  );

  return $batch;
}

/**
 * Batch operation for re-syncing content.
 *
 * Update Node, Taxonomy or User objects.
 */
function _drupal_ddp_batch_sync_content_operation($id, $args, $operation_details, &$context) {

  switch ($args[1]) {
    case 'node':
      $node = node_load($id, NULL, TRUE);
      // Save node.
      node_save($node);

      $title = $node->title;
      break;

    case 'taxonomy':
      $term = taxonomy_term_load($id);
      // Save taxonomy term.
      taxonomy_term_save($term);

      $title = $term->name;
      break;

    case 'user':
      $user = user_load($id);
      // Save User object.
      user_save($user);

      $title = $user->name;
      break;
  }

  $context['results'][] = $id . ' : ' . check_plain($title);

  // Message displayed under the progress bar.
  $context['message'] = t('Syncing @type "@title" @operation_detail',
    array(
      '@title' => $title,
      '@type' => $args[1],
      '@operation_detail' => $operation_details,
    )
  );

  _drupal_ddp_update_http_requests();
}
