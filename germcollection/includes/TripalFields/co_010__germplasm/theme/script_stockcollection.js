/**
 * @file 
 * Script containing events from stock collection field widget.
 */

(function ($) {
  Drupal.behaviors.scriptStockCollection = {
    attach: function (context, settings) {   
    
    // Select field value when on focus.
    var fldGermplasm = $('#stock-collection-fld-textfield-germplasm');
    fldGermplasm.click(function() {
        $(this).select();
      });
    
    // Reset germplasm field with organism select field reset.
    $('#stock-collection-fld-select-species')
      .change(function() {
        if ($(this).val() == 0) {
          fldGermplasm.val('');
        }
      });

    // Expand bulk upload.
    $('#stock-collection-bulk-upload a').once(function() {
      $(this).click(function(e) {
        e.preventDefault();

        $(this).text(function() {
          return ($(this).text() == 'Upload File') 
            ? 'Cancel File Upload' : 'Upload File';
        });

        $('#stock-collection-container-bulk-upload').slideToggle(200);
      });
    });

}};}(jQuery));