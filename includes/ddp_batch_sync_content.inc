<?php

/**
 * @file
 *
 * File for batch processing the sending of
 * nodes from Drupal to Meteor.
 */

/**
 * Get all nids for a given content type.
 *
 * @return array
 *  Returns array of node ids for given content type.
 */
function drupal_ddp_get_all_nids_of_type($content_type) {
  $result = db_query("SELECT nid FROM node WHERE type = :s", array(':s' => $content_type));
  $nids = array_keys($result->fetchAllAssoc('nid'));

  return $nids;
}

/**
 * Sync existing node data with meteor app.
 */
function drupal_ddp_sync_existing_nodes($form, &$form_state) {
  // If value is array, then no value was chosen.
  if(is_array($form_state['values']['content_type'])) {
    drupal_set_message(t('No content types were chosen.'), 'error');
    return;
  }
  $content_type_to_sync = $form_state['values']['content_type'];

  $function = 'batch_drupal_ddp_sync_nodes';

  // Reset counter for debug information.
  $_SESSION['http_request_count'] = 0;

  // Execute the function named batch_drupal_ddp_sync_nodes.
  $batch = drupal_ddp_batch_sync_nodes($content_type_to_sync);
  batch_set($batch);
}

/**
 * Batch Definition: Load and save all nodes with particular type.
 */
function drupal_ddp_batch_sync_nodes($content_type) {
  $nids = drupal_ddp_get_all_nids_of_type($content_type);
  $num_operations = count($nids);
  drupal_set_message(t('Syncing @num nodes of "@content_type"', array('@num' => $num_operations, '@content_type' => $content_type)));

  $operations = array();
  // Each operation in the operations array means at least one new HTTP request,
  // running Drupal from scratch to accomplish the operation. If the operation
  // returns with $context['finished'] != TRUE, then it will be called again.
  // In this example, $context['finished'] is always TRUE.
  for ($i = 0; $i < $num_operations; $i++) {
    // Each operation is an array consisting of
    // - The function to call.
    // - An array of arguments to that function.
    $operations[] = array(
      'drupal_ddp_batch_operation',
      array(
        $nids[$i],
        t('(Operation @operation)', array('@operation' => $i + 1)),
      ),
    );
  }
  $batch = array(
    'operations' => $operations,
    'finished' => 'drupal_ddp_batch_finished',
  );
  return $batch;
}

/**
 * Batch operation for syncing nodes.
 *
 * Load a node and save it to invoke hook_entity_update().
 */
function drupal_ddp_batch_operation($nid, $operation_details, &$context) {
  $node = node_load($nid, NULL, TRUE);

  // Store some results for post-processing in the 'finished' callback.
  // The contents of 'results' will be available as $results in the
  // 'finished' function (in this example, batch_example_finished()).
  $context['results'][] = $node->nid . ' : ' . check_plain($node->title);

  // Optional message displayed under the progressbar.
  $context['message'] = t('Syncing node "@title"', array('@title' => $node->title)) . ' ' . $operation_details;

  // Save node.
  node_save($node);

  _drupal_ddp_update_http_requests();
}

/**
 * Batch 'finished' callback used by both batch 1 and batch 2.
 */
function drupal_ddp_batch_finished($success, $results, $operations) {
  if ($success) {
    // Here we could do something meaningful with the results.
    // We just display the number of nodes we processed...
    drupal_set_message(t('@count nodes synced.', array('@count' => count($results))));
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(
      t('An error occurred while processing @operation with arguments : @args',
        array(
          '@operation' => $error_operation[0],
          '@args' => print_r($error_operation[0], TRUE),
        )
      )
    );
  }
}

/**
 * Utility function to increment HTTP requests in a session variable.
 */
function _drupal_ddp_update_http_requests() {
  $_SESSION['http_request_count']++;
}

/**
 * Utility function to count the HTTP requests in a session variable.
 * 
 * @return int
 *   Number of requests.
 */
function _drupal_ddp_get_http_requests() {
  return !empty($_SESSION['http_request_count']) ? $_SESSION['http_request_count'] : 0;
}