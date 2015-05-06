(function ($) {

  Drupal.behaviors.tripalGermplasm_StockRelationshipJump = {
    attach: function (context) {

    // When a parental remove button is clicked we want to
    // highlight & scroll to the parent section
    // and tell them to do this here instead.
    $('tr.relationship.custom-control input').click(function(event) {
      event.preventDefault();

      // Scroll to parent fieldset.
      $('html, body').animate({
        scrollTop: $('fieldset#edit-parents').offset().top -50
      }, 500);

      // Add class to allow highlighting
      $('fieldset#edit-parents').addClass('highlight-fieldset');

      // Add message telling the user how to remove.
      $('.tripal-germplasm-warning.fieldset-warning.relationship-parent').show();
    });

    // Remove highlight when they appear to have gotten the point.
    $('fieldset#edit-parents input').focus(function() {
      $('fieldset#edit-parents').removeClass('highlight-fieldset');
    });
  }};
})(jQuery);