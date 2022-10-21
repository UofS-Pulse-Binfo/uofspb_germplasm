/**
 * @file 
 * Script containing events from stock collection field widget.
 */

(function ($) {
  Drupal.behaviors.scriptStockCollection = {
    attach: function (context, settings) {   
    // Organism select.
    var selOrganism = $('#stock-collection-fld-select-species');
    // Germplasm field.
    var fldGermplasm = $('#stock-collection-fld-textfield-germplasm');
    // Host address.
    var host = window.location.protocol + '//' + window.location.hostname;


    // Select text value of germplasm field when clicked.
    fldGermplasm.click(function() {
      $(this).select();
    });
    
    // Clear the value of germplasm field when organism
    // has changed value. 
    selOrganism.change(function() {  
      fldGermplasm.val('');
    });

    // Expand bulk upload.
    $('#stock-collection-bulk-upload a').once(function() {
      $(this).click(function(e) {
        e.preventDefault();
        
        $(this).text(function() {
          return ($(this).text() == 'Upload File') 
            ? 'Cancel File Upload' : 'Upload File';
        });

        $('#stock-collection-container-bulk-upload')
          .slideToggle(200);
      });
    });

    // Insert germplasm to collection.
    $('#stock-collection-fld-button-add').once(function() {
      $(this).click(function(e) {
        e.preventDefault();
        
        // Gemplasm field value returned/suggested by autocomplete
        // comprises of the stock id and the stock name separated by
        // colon : character. ie 111:stock 1.
        var fld = $('#stock-collection-fld-textfield-germplasm');
        var curVal = fld.val();
        var e = curVal.split(':');
        
        // Inspect the id if it is currently in the collection.
        var isIn = stockCollection(e[0], 'exists');

        if (!isIn && parseInt(e[0]) > 0 && e[1] != '') {
          // Save.
          stockCollection(e[0], 'insert');

          // Clear field.
          fld.val('');
          getStockProfile(e[0]);
        }
      });
    });
    
    // Remove germplasm from collection.
    // This is dynamically created element.
    /*
    document.addEventListener('click', function (e) {
      if (e.target.classList.contains('stock-collection-remove')) {
        var tableRowId = e.target.className.replace('remove stock-', '');
        $('#' + tableRowId).remove(); 
      }
    }, false); */

    fldGermplasm.autocomplete({     
      source: function(request, response) {
        if (selOrganism.val() != 0) {
          $.getJSON(host + Drupal.settings.germcollection.path.autocomplete + selOrganism.val() + '/' + request.term, {
            term:request.term }, function(data) {
              response($.map(data, function(item) {
                return {
                  label: item.name,
                  value: item.id
                }
              }))
          });
        }
      },
    });
    
    
    /////
    

    /**
     * Post stock in stock collection table.
     * 
     * @param data
     *   Object - stock properties:
     *   - stock_id: id number of germplasm
     *   - name: name of germplasm
     *   - organism: genus + species 
     *   - entity: entity id 
     */
    function postStock(data) {
      var table = $('#stock-collection-germplasm-container table');  
      // Create an alternating bg colour for each row.
      var bg = (table.find('tr').length % 2) ? '#FBFBFB' : '#F7F7F7';
      // Germplasm table row - id using the stock id for quick reference.
      var markup = '<tr id="stock-collection-' + data.stock_id + '" style="background-color: ' + bg + '"><td>name</td><td>del</td></tr>';

      // NAME + ORGANISM:
      // In some cases stocks have no associated entity id,
      // show stock as plain text when this is true.
      var name = '';
      if (data.entity == '#') {
        // no link to bio data page.
        name = data.name;
      }
      else {
        // has bio data page.
        name = '<a href="' + host +  Drupal.settings.germcollection.path.biodata + data.entity +'" target="_blank">' + data.name + '</a>';
      }
      
      name += ' : ' + data.organism;
      markup = markup.replace('name', name);

      // DELETE FROM COLLECTION:
      // Tag each line with the same stock id used in each row
      // and use this id to construct id of selected row for deletion.
      var del = '';
      del = '<a href="#" class="stock-collection-remove stock-' + data.stock_id + '">&#x2715;</a>';
      markup = markup.replace('del', del);

      // Add row.
      table.append(markup);
    }

    /**
     * Get stock profile.
     * 
     * @para id
     *   Integer, id number of germplasm as returned by germplasm
     *   field in the interface.
     */
    function getStockProfile(id) {
      // Double check to see the id is active and currently
      // in the collection listing.
      var collectionVal = $('#stock-collection-set-id')
        .val().split(':');
      
      if (collectionVal.includes(id)) {
        $.ajax({
          url: host + Drupal.settings.germcollection.path.stockprop + id,
          progress: {type: 'throbber'},  
          success: function(data) {
            // Render row into the frontend.
            postStock(data);
          }
        });
      }
    }

    /**
     * Inspect if germplasm has been added previously.
     * 
     * @param id
     *   Integer, id number of germplasm as returned by germplasm
     *   field in the interface.
     * @param action
     *   String, which action to execute on a given germplasm.
     *   - exists: check if germplasm is already in the collection or not.
     *   - insert: insert germplasm to collection.
     *   - remove: remove germplasm from collection.
     */
    function stockCollection(id, action) {
      // Field to hold germplasm entered into collection.
      var collection = $('#stock-collection-set-id');
      var collectionVal = collection.val();

      if (action == 'exists') {
        // Unique ids.
        return collectionVal.split(':')
          .includes(id);
      }
      else if(action == 'insert') {
        // Insert to collection.
        var v = (collectionVal == 0) ? id : collectionVal + ':' + id;
        collection.val(v);
      }
      else if (action == 'remove') {
        // Remove from collection.
        var ids = collectionVal.split(':');
        var i = ids.indexOf(id);

        if (i > -1) {
          ids.splice(i, 1);
          var restore = ids.join(':');
          collection.val(restore);
        }
      }
    }

}};}(jQuery));