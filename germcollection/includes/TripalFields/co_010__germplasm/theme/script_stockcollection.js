/**
 * @file 
 * Script containing events from stock collection field widget.
 */
(function ($) {
  Drupal.behaviors.scriptStockCollection = {
    attach: function (context, settings) {   
    
    // Host.
    var host = window.location.protocol + '//' + window.location.hostname;
    
    // Expand bulk upload window.
    $('#stock-collection-bulk-upload a').once(function() {
      $(this).click(function(e) {
        e.preventDefault();
        
        $(this).text(function() {
          return ($(this).text() == 'Upload File') 
            ? 'Cancel File Upload' : 'Upload File';
        });

        $('#stock-collection-container-bulk-upload')
          .slideToggle();
      });
    });


    // Reference elements.
    // Organism select.
    var selOrganism = $('#stock-collection-fld-select-species');
    // Germplasm field.
    var fldNameGermplasm = 'stock-collection-fld-textfield-germplasm';
    var fldGermplasm = $('#' + fldNameGermplasm);
    // Germplasm collection field.
    var fldCollection = $('#stock-collection-set-id');

    // Load germplasm when collection field has value.
    $(document).ready(function() {
      $(this).once(function() {
      if (fldCollection.val() != 0) {
        var e = fldCollection.val().split(':');
        if (e.length > 0) {
          $.each(e, function(i, v) {
            var isIn = stockCollection(v, 'exits');
            console.log(isIn);
            if (!isIn) {
              stockPost(v);
            }
          });
        }
      }
      });
    });

    // Select text value of germplasm field when clicked
    // to assist quick modification/replacement of search text.
    fldGermplasm.click(function(e) {
      $(this).select();
    });
    
    // Clear the value of germplasm field when organism
    // has changed value to assist quick entry of search text.
    // relating to the current organism.
    selOrganism.change(function() {  
      fldGermplasm.val('');
    });

    // Autocomplete search germplasm.
    fldGermplasm
      .removeClass('form-text')
      .addClass('stock-collection-autocomplete-inactive')
      .autocomplete({     
        source: function(request, response) {
          $.getJSON(host + Drupal.settings.germcollectionPath.autocomplete 
            + selOrganism.val() 
            + '/' 
            + request.term, {
            term:request.term }, function(data) {
              response($.map(data, function(item) {
                return (item) ? { label: item.name, value: item.id } : false;
              }))
          });
        },
      });

    // Insert germplasm to collection.
    $('#stock-collection-fld-button-add').once(function() {
      $(this).click(function(e) {
        e.preventDefault();
        
        // Gemplasm field value returned/suggested by autocomplete
        // comprises of the stock id and the stock name separated by
        // colon : character. ie 111:stock 1 where 111 is the stock id 
        // and stock 1 is the stock name.
        var fld = $('#' + fldNameGermplasm);
        var curVal = fld.val();
        var e = curVal.split(':');
      
        if (e[1] != '' && parseInt(e[0]) > 0) {
          // Has stock id and stock name - is a valid entry.
          // Inspect the id if it is currently in the collection.
          var isIn = stockCollection(e[0], 'exists');
          
          if (!isIn) {
            // Clear field.
            fld.val('');

            var id = e[0];
            stockPost(id);
          }
        }
      });
    });

    // Remove germplasm from collection.
    // This is dynamically created element.
    document.addEventListener('click', function (e) {
      if (e.target.classList.contains('stock-collection-remove')) {
        e.preventDefault();

        if (confirm('Are you sure you want to remove?') == true) {
          // Removing germplasm from collection.
          // Prepare stock id encoded in the class.
          // stock-collection-stock-id.
          var tableRowId = e.target.className.replace('remove stock-', '');
          var id = tableRowId.replace('stock-collection-', '').trim(); 
          
          // See if this id does exist in the collection
          // before removing.
          var isIn = stockCollection(id, 'exists');
          
          if (isIn) {
            // Remove from collection field.
            stockCollection(id, 'remove');
            // Remove table row.
            $('#' + tableRowId).remove(); 
            // Decrement germplasm count.
            stockCount('decrement');
          }
        }
      }
    });


    ////
  

    /**
     * Increment/decrement germplasm count as
     * item being added or removed from collection.
     * 
     * @param action
     *   - increment: add 1.
     *   - decrement: less 1.
     */
    function stockCount(action) {
      var e = $('#stock-collection-header span');
      var count = e.text();

      if (action == 'increment') {
        count = parseInt(count) + 1;
      }
      else if (count > 0 && action == 'decrement') {
        count = parseInt(count) - 1;
      }

      e.text(count);
    }

    /**
     * Manage stock collection field that holds final
     * stock id to be inserted into stock collection table.
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
        var isIn = stockCollection(id, 'exists');
        
        if (!isIn) {
          var v = (collectionVal == 0) ? id : collectionVal + ':' + id;
          collection.val(v);
        }
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

    /**
     * Post germplsm to collection.
     * 
     * @param id
     *   Integer, id number of germplasm as returned by germplasm
     *   field in the interface. 
     */
    function stockPost(id) {
      // Tell the user to wait and disable add button.
      var waitWindow = $('#stock-collection-wait');
      waitWindow.text('Adding germplasm, please wait...');

      $.ajax({
        url: host + Drupal.settings.germcollectionPath.stockprop,
        dataType: 'json',
        data: {'id': id},
        complete: function() {
          waitWindow.text(''); 
        },
        success: function(data) {
          if (data.length != 0) {
            // Saving germplasm to collection...
            // Insert into collection field.
            stockCollection(id, 'insert');
            
            // Post row into the frontend.
            var table = $('#stock-collection-germplasm-container table');  
            // Create an alternating bg colour for each row.
            var bg = (table.find('tr').length % 2) ? '#FBFBFB' : '#F7F7F7';
            // Germplasm table row - id using the stock id for quick reference.
            var markup = '<tr id="stock-collection-' 
              + data.stock_id 
              + '" style="background-color: ' 
              + bg 
              + '"><td>@name</td><td>@remove</td></tr>';
    
            // NAME + ORGANISM:
            // In some cases stocks have no associated entity id,
            // show stock as plain text when this is true.
            var name = (data.entity == '#') ? data.name :
              '<a href="' 
                + host 
                + Drupal.settings.germcollectionPath.biodata 
                + data.entity 
                +'" target="_blank">' 
                + data.name 
                + '</a>';
          
            name += ' : ' + '<small>' + data.organism + '</small>';
            markup = markup.replace('@name', name);
    
            // DELETE FROM COLLECTION:
            // Tag each line with the same stock id used in each row
            // and use this id to construct id of selected row for deletion.
            var remove = '<a href="#" class="stock-collection-remove stock-' 
              + data.stock_id 
              + '">&#x2715;</a>';
            
            markup = markup.replace('@remove', remove);
    
            // Add row.
            table.prepend(markup);

            // Increment gerplasm count.
            stockCount('increment');
          }
        },
        error: function() {
          console.log('Not a valid germplasm');
        }
      });
    }

}};}(jQuery));