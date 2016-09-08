<?php
  /**
   * @file
   * Master template file of Wild Germ Summary Module.
   *
   * Available variables:
   * -
   */

   // Template structure:
   // <div>           - Main div container
   //   <div>         - div container for matrix
   //     <div></div> - Maternal Parent caption
   //     Matrix      - The matrix
   //   </div>
   //
   //   <div>         - Container for summary table
   //     <div></div> - div container for summary table title
   //     <div></div> - Horizontal line
   //     Table       - The table
   //   </div>
   // </div>

  // Determine if table is present do not show the matrix.
  $display_matrix = '';
  if (isset($form['siblings_mom']['#value']) AND isset($form['siblings_dad']['#value'])) {
    $display_matrix = 'no-display';
  }
?>

<div id="container-summary">

  <?php print drupal_render($form['description']); ?>

  <div id="container-matrix" class="<?php print $display_matrix; ?>">
    <div class="table-caption horizontal-col">&nbsp;</div>
    <?php print drupal_render($form['summary_matrix']); ?>
  </div>

  <?php if ($display_matrix == 'no-display') { ?>
    <div id="container-table">
      <div>
        <ul id="table-title-cells">
          <li><?php print $form['siblings_mom']['#value']; ?><small>Maternal Parent</small></li>
          <li><?php print $form['siblings_dad']['#value']; ?><small>Paternal Parent</small></li>
          <li><?php print $form['siblings_count']['#value']; ?> Stock Records<small>&nbsp;</small></li>
        </ul>

        <div class="nav-link"><a href="#">Wild Lentil Crosses Matrix</a></div>
      </div>

      <div class="div-line">&nbsp;</div>
      <?php print drupal_render($form['summary_table']); ?>
    </div>
  <?php } ?>

</div>
