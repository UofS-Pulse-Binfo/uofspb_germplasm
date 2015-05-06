(function ($) {

  Drupal.behaviors.tripalGermplasm_StockPropertyJump = {
    attach: function (context) {

    // When a property remove button is clicked we want to
    // highlight & scroll to the type-specific property section
    // and tell them to do this here instead.
    $('tr.property.custom-control input').click(function(event) {
      event.preventDefault();

      // Scroll to parent fieldset.
      $('html, body').animate({
        scrollTop: $('#type-specific-options').offset().top -50
      }, 500);

      // Add class to allow highlighting
      $('#type-specific-options fieldset').addClass('highlight-fieldset');

      // Add message telling the user how to remove.
      $('.tripal-germplasm-warning.fieldset-warning.property-type-specific').show();
    });

    // Remove highlight when they appear to have gotten the point.
    $('#type-specific-options fieldset input').focus(function() {
      $('#type-specific-options fieldset').removeClass('highlight-fieldset');
    });
  }};
})(jQuery);