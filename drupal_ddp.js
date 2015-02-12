/**
 * @file
 * Drupal DDP Javascript file.
 */

/**
 * Update the summary for the module's vertical tab.
 */
(function ($) {
  Drupal.behaviors.vertical_tabs_ddp_summary = {
    attach: function (context) {
      // Use the fieldset class to identify the vertical tab element.
      $('fieldset#edit-drupal-ddp', context).drupalSetSummary(function (context) {
        // Depending on the checkbox status, the settings will be customized, so
        // update the summary with the custom setting textfield string or a use a
        // default string.
        if ($('#edit-drupal-ddp-enabled', context).attr('checked')) {
          return Drupal.t('Use with DDP');
        }
        else {
          return Drupal.t('');
        }
      });
    }
  };
})(jQuery);
