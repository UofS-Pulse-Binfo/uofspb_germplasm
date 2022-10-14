/**
 * @file 
 * Script containing events from stock collection field widget.
 */

(function ($) {
  Drupal.behaviors.scriptStockCollection = {
    attach: function (context, settings) {   
      var organism;

      $('#stock-collection-fld-select-species').change(function() {
        console.log($(this).val());
        organism = $(this).val();
      });
      
      $('#stock-collection-fld-textfield-germplasm')
      .autocomplete({
        source: function(request, response) {
          $.ajax({
            url: 'germcollection/autocomplete-stockname/' + organism,
            dataType: 'json',
            data: {
              term: request.term
            },
            success: function(data) {
              response(data);
            }
          })  
        }
    });
    


}};}(jQuery));