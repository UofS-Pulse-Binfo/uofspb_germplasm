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

  <div id="container-matrix" class="<?php print $display_matrix; ?> paternal-bg">
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

        <div class="nav-link"><?php print drupal_render($form['back_link']); ?></div>
      </div>

      <div class="div-line">&nbsp;</div>
      <?php print drupal_render($form['summary_table']); ?>
    </div>
  <?php }
        else {
            print tripal_set_message('In order for a RIL to appear in this chart, it must be in the stock table with type "414 inbred line (CO_010:0000162)".',
              TRIPAL_NOTICE, ['return_html' => TRUE]);
            print tripal_set_message('RILs will show up with unknown parents unless they have an original cross and parents as specified <a href="https://kp-germplasm.readthedocs.io/en/latest/rilsummary/rilsummary.html#adding-rils-to-the-summary">here</a>.',
              TRIPAL_NOTICE, ['return_html' => TRUE]);
        } ?>

</div>
