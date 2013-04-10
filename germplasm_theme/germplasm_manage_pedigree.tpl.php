<div id="tripal_stock-germplasm_pedigree-box" class="germplasm_manage-info-box tripal-info-box">
  <div class="germplasm_manage-info-box-title tripal-info-box-title">Pedigree</div>

  <div class="views-exposed-widget">
    <?php
      print drupal_get_form('germplasm_manage_generate_pedigree_on_node_form', $node->stock->stock_id);
    ?>
  </div>

  <?php
    $pedigree_type = ($_GET['pedigree_type']) ? $_GET['pedigree_type'] : 'subject';
    $num_levels = ($_GET['generation'])? $_GET['generation'] : 3;

    if ($pedigree_type == 'object') {
      $pedigree = germplasm_manage_generate_object_pedigree_array($node->stock->stock_id, $num_levels);
    } else {
      $pedigree = germplasm_manage_generate_subject_pedigree_array($node->stock->stock_id, $num_levels);
      $pedigree_string = germplasm_manage_generate_textual_pedigree_string($pedigree, $num_levels);
    }

  ?>
  <p><th>The pedigree that is depicted below is a representation of the lineage or genealogical descent of the selected stock.
  The lineage has been traced to <?php print $_GET['generation']; ?> generations. </th></p><br>

  <p><strong>Pedigree: </strong>
    <?php if (!empty($pedigree_string)) { ?>
      <?php print $pedigree_string; ?></p>
    <?php } else { ?>
      Unknown</p>
    <?php } ?>
  </table>
  <?php //print theme('germplasm_manage_pedigree_drawing', $pedigree); ?>
</div>


