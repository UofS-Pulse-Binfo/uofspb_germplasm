/**
 * @file 
 * Script containing events from stock collection field widget.
 */
(function ($) {
  Drupal.behaviors.scriptCollectionCreate = {
    attach: function (context, settings) {   
    // Reference elements.
    var mainContainer = $('#germcollection-collection-create-container');
    // Organism select.
    var selOrganism = $('#germcollection-fld-select-species');
    // Germplasm field.
    var fldNameGermplasm = 'germcollection-fld-textfield-germplasm';
    var fldGermplasm = $('#' + fldNameGermplasm);
    // Germplasm collection field.
    var fldCollection = $('#germcollection-set-id');
    // Host.
    var host = Drupal.settings.germcollectionPath.host;
    
    // Expand/collapse bulk upload window.
    // Expand/collapse collection table.
    mainContainer.find('span a').once(function() {
      $(this).click(function(e) {
        e.preventDefault();
        
        var collectionWindow = $('#germcollection-collection-table');
        var uploadWindow     = $('#germcollection-bulk-upload'); 
        var states = {
          text : 'Cancel File Upload',
          fldDisabled : 'disabled',
          fldBg : '#F7F7F7'
        };

        if ($(this).text() == 'Upload File') {
          // Collapse collection window and load upload window.
          // Reset select and autocomplete fields:
          selOrganism.val(0);
          fldGermplasm.val('');

          // Clear items entered previously.
          if (fldCollection.val() != 0) { 
            stockCount('reset');
            $('#germcollection-collection-table table')
              .empty();
            
            fldCollection.val('0');
          }

          collectionWindow.slideUp();
          uploadWindow.slideDown();
        }
        else {
          // See if there is undeleted file in the field and
          // let user clear this before loading collecion window.
          if ($('.file a').length > 0) {
            alert('Please remove file and try again.')
          }
          else {
            // Expand collection window and collapse upload window.
            states.text = 'Upload File';
            states.fldDisabled = '';
            states.fldBg = '#FFFFFF';

            uploadWindow.slideUp();
            collectionWindow.slideDown();
          }
        }

        // Update link text.
        $(this).text(states.text);

        // Disable fields.
        selOrganism.attr('disabled', states.fldDisabled)
          .css('background-color', states.fldBg);

        fldGermplasm.attr('disabled', states.fldDisabled)
          .css('background-color', states.fldBg); 
      });
    });

    // Load germplasm when collection field has value.
    $(document).ready(function() {
      fldCollection.once(function() {
        if (fldCollection.val() != 0) { 
          var e = fldCollection.val().split(':');
          if (e.length > 0) {
            $.each(e, function(i, v) {
              var isIn = stockCollection(v, 'exits');
              
              if (!isIn) {
                stockPost(v);
              }
            });
          }
        }
      });

      mainContainer.find('span a').once(function(e) {
        if ($(this).text() == 'Upload File'  && $('.file a').length > 0) {  
          $(this).click();
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
      .addClass('germcollection-autocomplete-inactive')
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
    $('#germcollection-fld-button-add').once(function() {
      $(this).click(function(e) {
        e.preventDefault();
        
        // Gemplasm field value returned/suggested by autocomplete
        // comprises of the stock id and the stock name in the following notation:
        // stock name [id:stock id number].
        var fld = $('#' + fldNameGermplasm);
        var curVal = fld.val();
        // Remove brackets and split string by id: isolating
        // the stock name and the stock id number.
        var e = curVal
          .replace(/[\[\]]/g, '')
          .split('id:');

        if (e[0].trim() != '' && e[1].match(/^[0-9]+$/) && parseInt(e[1]) > 0) {
          // Has stock id and stock name - is a valid entry.
          // Inspect the id if it is currently in the collection.
          var isIn = stockCollection(e[1], 'exists');
          
          if (!isIn) {
            // Clear field.
            fld.val('');

            var id = e[1];
            stockPost(id);

            // Put the cursor back to
            // germplasm field.
            fld.focus();
          }
        }
      });
    });

    // Remove germplasm from collection.
    // This is dynamically created element.
    document.addEventListener('click', function (e) {
      if (e.target.classList.contains('germcollection-remove')) {
        e.preventDefault();
        
        if (confirm('Are you sure you want to remove?') == true) {
          // Removing germplasm from collection.
          // Prepare stock id encoded in the class.
          // stock-collection-stock-id.
          var tableRowId = e.target.className.replace('remove stock-', '');
          var id = tableRowId.replace('germcollection-', '').trim(); 
          
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
    }, false, {once : true});
    

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
      var e = $('#germcollection-collection-header span');
      var count = e.text();

      if (action == 'increment') {
        count = parseInt(count) + 1;
      }
      else if (count > 0 && action == 'decrement') {
        count = parseInt(count) - 1;
      }
      else if (action == 'reset') {
        count = 0;
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
      var collection = $('#germcollection-set-id');
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
          var checkVal = (restore.length > 0) ? restore : 0;
          collection.val(checkVal);
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
      var id = parseInt(id);

      // Tell the user to wait and disable add button.
      var waitWindow = $('#germcollection-collection-header div');
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
            var table = $('#germcollection-collection-table table');  
            // Create an alternating bg colour for each row.
            var bg = (table.find('tr').length % 2) ? '#FBFBFB' : '#F7F7F7';
            // Germplasm table row - id using the stock id for quick reference.
            var markup = '<tr id="germcollection-' 
              + data.stock_id 
              + '" style="background-color: ' 
              + bg 
              + '"><td>@name</td><td>@publish</td><td>@remove</td></tr>';
    
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
            
            // PUBLISHED or UNPUBLISHED:
            // If a germplasm is unpublished or has no Tripal Content Type
            // page setup, inform collection author accordingly.
            var unpubNote = 'Unpublished Germplasm has no page and may be omitted from view list or exported file.';
            var publish = (data.entity == '#') ? '<em title="' + unpubNote + '">[unpublished]</em>' : ''; 
            markup = markup.replace('@publish', publish);

            // DELETE FROM COLLECTION:
            // Tag each line with the same stock id used in each row
            // and use this id to construct id of selected row for deletion.
            var remove = '<a href="#" class="germcollection-remove stock-' 
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