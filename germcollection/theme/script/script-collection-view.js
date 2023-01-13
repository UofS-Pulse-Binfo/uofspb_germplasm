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
      .addClass('germcollection-autocomplete-inactive')
      .autocomplete({
        
      });

  }};}(jQuery));