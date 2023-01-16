/**
 * @file 
 * Script containing events from stock collection field widget.
 */
(function ($) {
  Drupal.behaviors.scriptCollectionView = {
    attach: function (context, settings) {   
    // Search box  
    var fldNameSearch = 'germcollection-fld-textfield-search';
    var fldSearch = $('#' + fldNameSearch);
    // Host.
    var host = Drupal.settings.germcollection.host;  
    
    // Select text value of germplasm field when clicked
    // to assist quick modification/replacement of search text.
    fldSearch.click(function(e) {
      $(this).select();
    });
    
    // Autocomplete search germplasm in a collection.
    fldSearch
      .removeClass('form-text')
      .addClass('germcollection-autocomplete-inactive')
      .autocomplete({
        source: function(request, response) {
          $.getJSON(host + Drupal.settings.germcollection.autocomplete 
            + request.term, {
          term:request.term }, function(data) {
            response($.map(data, function(item) {
              return (item) ? { label: item.name, value: item.id } : false;
            }))
          });
        },
      });
    
    $('#germcollection-search-submit').click(function() {
      if (fldSearch.val()) {
        $('#germcollection-form').submit();
      }
    });


  }};}(jQuery));