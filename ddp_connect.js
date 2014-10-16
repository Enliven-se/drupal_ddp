(function ($) {

/**
 * Update the summary for the module's vertical tab.
 */
Drupal.behaviors.vertical_tabs_exampleFieldsetSummaries = {
  attach: function (context) {
    // Use the fieldset class to identify the vertical tab element
    $('fieldset#edit-ddp-connect', context).drupalSetSummary(function (context) {
      // Depending on the checkbox status, the settings will be customized, so
      // update the summary with the custom setting textfield string or a use a
      // default string.
      if ($('#edit-ddp-connect-enabled', context).attr('checked')) {
        return Drupal.t('Use with DDP');
      }
      else {
        return Drupal.t('');
      }
    });
  }
};

})(jQuery);