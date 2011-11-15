<style type='text/css'>
  #canvas_container {
    border: 1px solid #aaa;
    background-color:white;
  }
</style>

<?php 
  // only create the div for the pedigree tree if BOTH of the following ARE FALSE:
  //	1) the pedigree tree is empty
  //	2) the pedigree tree consists of one stock (the current stock) with no children
  if (!empty($pedigree) AND !(sizeof($pedigree)==1 AND sizeof($pedigree['children'])==0)) { 
?>
  <div id='canvas_container'></div>
<?php } else { 
  $pedigree = array();
?>
  <div class="tripal-no-results">Cannot draw a pedigree for the current stock since there are no suitable relationships.</div>
<?php } ?>

<script type='text/javascript'>
  if (Drupal.jsEnabled) { 
    var rawtree;
    //javascript version of the php $pedigree array
    <?php print theme('germplasm_manage_pedigree_jsarray', $pedigree); ?>

    var resize_pedigree_tree = function (){
      var new_width = window.tripal_expandable_box.offsetWidth;
      window.paper.setSize(new_width, 500);
      window.paper.clear();
      window.tree.rawtree = window.rawtree;
      window.tree.generate_tree();
      window.tree.print_relationship_color_key(tripal_expandable_box.offsetWidth - 10,490, ['right', 'bottom']);
    }

    // Initialize the pedigree tree (don't actually draw it)
    // but only if there is a pedigree tree to draw
    if (rawtree.length > 0) {
      
      var tripal_expandable_box = document.getElementById('tripal_stock-germplasm_pedigree-box');
      var paper = new Raphael(document.getElementById('canvas_container'), window.tripal_expandable_box.offsetWidth, 500);
      var tree = new pedigree_tree(paper, rawtree);

      // ensure the tree is redrawn if the window is resized
      document.body.onresize = resize_pedigree_tree;
      
      // only draw the tree if the germplasm_pedigree tripal expandable box is visible
      // allow 50 ms to pass in order for other javascript to finish running
      setTimeout('draw_pedigree_tree_on_toc_click();',50);
      draw_pedigree_tree_on_toc_click = function () {
        //check if it's visible when the page is first loaded
        // this is the case if block=germplasm_pedigree is in the url
        if( $('#tripal_stock-germplasm_pedigree-box').is(':visible') ) {
          resize_pedigree_tree();
        } else {
          //else check again whenever any TOC link is clicked
          $(".tripal_stock_toc_item").click(function(){
            if( $('#tripal_stock-germplasm_pedigree-box').is(':visible') ) {
              resize_pedigree_tree();
            }
          });
        }
      }
    }
  }
</script>


