/**
 * @file
 * Handle events in summary matrix
 */

(function($) {
  Drupal.behaviors.summaryMatrixBehaviour = {
    attach: function (context, settings) {
      // Fix Drupal cloning the header of table
      // and duplicates all element id attribute.
      $('.sticky-header th').each(function() {
        var cur_id = $(this).attr('id');
        var new_id = cur_id + '-sticky';
        $(this).attr('id', new_id);
      });

      // Manage maternal and paternal captions.
      var matrixContainer = $('div#container-matrix');

      $(document).scroll(function() {
        if ($('#container-matrix .sticky-header').css('visibility') == 'visible') {
          matrixContainer.removeClass('paternal-bg');
        }
        else {
          matrixContainer.addClass('paternal-bg');
        }
      });

      // Add event listener to table cells.
      $('#tbl-summary-matrix td')
      ////
        .mouseover(function() {
          // child object.
          var siblings = {};

          // Get the id attribute of the selected cell.
          // The id attribute contains parent cell index in table element.
          // eg. p_2_3 where the first digit is the mother and the second
          //     digit is the father.
          siblings.attrId = $(this).attr('id');
          var parents = siblings.attrId.split('-');
          // Get the index of parents;
          siblings.mom = parents[1];
          siblings.dad = parents[2];

          // Highlight sibling count cell and parents as well.
          highlightParentCells(siblings);
        })
        ////
        .mouseout(function() {
          // Remove all highlights to cell, parents and between.
          highlightParentCells(0);
        });

      // Function to highlight parents of siblings count.
      function highlightParentCells(siblings) {
        if (siblings) {
          // Style the child cell first.
          var theCell = $('#' + siblings.attrId);
          theCell.addClass('highlight-child');

          // Style the mom and dad cell.
          var momTh = $('#maternal-' + siblings.mom);
          momTh.addClass('highlight-mom');

          // For sticky header.
          var momThSticky = $('#maternal-' + siblings.mom + '-sticky');
          momThSticky.addClass('highlight-mom-sticky');

          momThSticky.css('color', 'red');

          var dadTh = $('#paternal-' + siblings.dad);
          dadTh.addClass('highlight-dad');

          // Create a beam effect from child cell to mom and dad cell.
          for(var k = siblings.dad - 1; k > 0; k--) {
            // Move one cell up.
            td = $('#paternal-' + k).siblings('td');
            $(td[siblings.mom - 1]).addClass('highlight-dad-cells');
          }

          var td = dadTh.siblings('td');
          for(var i = 0; i < siblings.mom; i++) {
            // Move one cell to the right.
            $(td[i - 1]).addClass('highlight-mom-cells');
          }
        }
        else {
          // Remove all highlighting classes attached to table headers and cells.
          $('th, td').removeClass (function (index, css) {
            return (css.match (/(^|\s)highlight-\S+/g) || []).join(' ');
          });
        }
      }
    }
  };
}(jQuery));
