(function ($) {

  Drupal.behaviors.tripalGermplasm_StockChadoNodeApiJump = {
    attach: function (context) {


    // REMOVE
    //-------------
    // Since ajax is fired to attach the error message we can't hook into the
    // click event. Thus we keep an eye out for any rows classed with the
    // error-highlight added by said ajax.
    if ($('tr.error-highlight').length > 0) {

      var fieldsetCategory = Drupal.settings.tripalGermplasm.correctFieldset;
      var fieldsetId = Drupal.settings.tripalGermplasm[fieldsetCategory];

      // Scroll to parent fieldset.
      $('html, body').animate({
        scrollTop: $(fieldsetId).offset().top -50
      }, 500);

      // Add class to allow highlighting
      $(fieldsetId).addClass('highlight-fieldset');

      // Highlight the existing instructions on how to remove properties.
      $(fieldsetId + ' .germplasm-instructions').addClass('highlight-text');

      // Highlight the field they should be using.
      $(Drupal.settings.tripalGermplasm.correctField).addClass('highlight-correct-field');
    }

    // ADD
    //-------------
    if (Drupal.settings.tripalGermplasm.preventAdd) {

        var fieldsetCategory = Drupal.settings.tripalGermplasm.correctFieldset;
        var fieldsetId = Drupal.settings.tripalGermplasm[fieldsetCategory];

        // Scroll to parent fieldset.
        $('html, body').animate({
          scrollTop: $(fieldsetId).offset().top -50
        }, 500);

        // Add class to allow highlighting.
        $(fieldsetId).addClass('highlight-fieldset');

        // Highlight the field they should be using.
        $(Drupal.settings.tripalGermplasm.correctField).addClass('highlight-correct-field');
    }

    // Remove highlight when they appear to have gotten the point.
    //-------------------------------------------------------------
    $('input.highlight-correct-field').focus(function() {
      var fieldsetId = '#' + $(this).closest('fieldset').attr('id');
      var fieldId = $(this).attr('id');

      $(fieldsetId).removeClass('highlight-fieldset');
      $('tr.error-highlight.' + fieldId).removeClass('error-highlight');
      $(fieldsetId + ' .germplasm-instructions').removeClass('highlight-text');
      $(this).removeClass('highlight-correct-field');
    });

  }};
})(jQuery);