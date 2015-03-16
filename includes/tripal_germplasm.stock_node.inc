<?php

/**
 * @file
 * Additions to the Chado Stock node type to support germplasm better.
 */

/**
 * Load germplasm specific information to a node.
 */
function tripal_germplasm_node_load(&$node) {

  $node->germplasm = new stdClass();

  // Add the maternal & paternal parent for display in the overview.
  $maternal = tripal_get_germplasm_parent('maternal',$node->stock);
  if (is_object($maternal)) {
    $node->germplasm->maternal_parent = $maternal;
  }
  $paternal = tripal_get_germplasm_parent('paternal',$node->stock);
  if (is_object($paternal)) {
    $node->germplasm->paternal_parent = $paternal;
  }

  // Add all the relationships
  $progeny = tripal_get_germplasm_progeny($node->stock, 'all');
  if (!empty($progeny)) {
    usort($progeny, "tripal_germplasm_cmp_progeny_name");
    $node->germplasm->progeny = $progeny;
  }

}

/**
 * Custom Sort: Sort progeny results by progeny name
 */
function tripal_germplasm_cmp_progeny_name($a, $b) {
  return strcmp($a['progeny']['name'], $b['progeny']['name']);
}