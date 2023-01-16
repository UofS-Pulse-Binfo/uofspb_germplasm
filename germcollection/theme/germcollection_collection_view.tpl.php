<?php

/**
 * @file
 * Template file to view germplasm collection.
 */

/**
 * Variables (key):
 * fld_no_per_page : field render array of number of germplasm select field.
 * context_links   : context links available in this page. 
 * germplasm_count : total germplasm found.
 * collection      : collection germplasm summary table (paged).
 */
?>

<div id="germcollection-collection-view-container">
  <form id="germcollection-form" method="post">
    <div id="germcollection-page-fields">
      <?php 
        // Field select # of germplasm per page.
        print drupal_render($variables['fld_no_per_page']); 

        // Field search box.
        print drupal_render($variables['fld_search']); 
      ?>

      <div id="germcollection-search-submit">&nbsp;</div>
    </div>
    
    <div id="germcollection-page-context">
      <?php 
        // Quick summary count of germplasm in a collection and
        // outbound link to search germplasm where the collection will 
        // be pre-selected on page load.
        print $variables['germplasm_count'] . ' Germplasm | ' . $variables['context_links']; 
      ?>
    </div>

    <div style="clear: left; height: 0">&nbsp;</div> 
  </form>
</div>
<div id="germcollection-collection-table">
  <?php 
    // Collection germplasm collection.
    print drupal_render($variables['collection_table']); 
  ?>
</div>