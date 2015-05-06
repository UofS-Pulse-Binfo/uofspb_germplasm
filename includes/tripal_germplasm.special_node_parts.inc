<?php

/**
 * @file
 */

/**
 * Define information about each special germplasm node part.
 *
 * Use this hook to define properties, dbxrefs and relationships that you
 * would like special form elements to be added for. This is meant to make the
 * stock form more friendly for entering germplasm.
 */
function tripal_germplasm_special_germplasm_node_details() {
  $parts = array();

  // Relationships
  //--------------------
  $parts['maternal_parent'] = array(
    'node_api_category' => 'relationship',
    'title' => 'Maternal Parent',
    'description' => 'The name of the maternal parent of this germplasm.',
    'form_type' => 'textfield',
    'type_id' => tripal_get_germplasm_parent_type_id('maternal'),
    'current_stock_side' => 'object',
    'category' => 'parent',
    'value_function' => 'tripal_germplasm_node_part_get_maternal_parent'
  );
  $parts['paternal_parent'] = array(
    'node_api_category' => 'relationship',
    'title' => 'Paternal Parent',
    'description' => 'The name of the paternal parent of this germplasm.',
    'form_type' => 'textfield',
    'type_id' => tripal_get_germplasm_parent_type_id('paternal'),
    'current_stock_side' => 'object',
    'category' => 'parent',
    'value_function' => 'tripal_germplasm_node_part_get_paternal_parent'
  );

  $type = tripal_get_cvterm(array('name' => 'is_selection_of'));
  $type_id = NULL;
  if (isset($type->cvterm_id)) {
    $type_id = $type->cvterm_id;
  }
  $parts['original_cross_number'] = array(
    'node_api_category' => 'relationship',
    'title' => 'Original Cross Number',
    'description' => 'The cross number the RIL originated from.',
    'form_type' => 'textfield',
    'type_id' => $type_id,
    'current_stock_side' => 'object',
    'category' => 'ril',
    'type_specific' => TRUE,
    'value_function' => 'tripal_germplasm_node_part_get_original_cross_number'
  );

  // Properties
  //--------------------

  return $parts;
}

/**
 * Retrieve the maternal parent for the given node.
 *
 * @param $node
 *   The node object of the child we want the maternal parent for.
 * @return
 *   An array containing the stock_id, nid, name and title of the maternal
 *   parent where the title is a link to the parent if possible.
 */
function tripal_germplasm_node_part_get_maternal_parent($node) {
  $return = array();

  $maternal_parent = tripal_get_germplasm_parent('maternal',$node->stock);
  if (is_object($maternal_parent)) {

    $return = array(
      'stock_id' => $maternal_parent->stock->stock_id,
      'name' => $maternal_parent->stock->name,
      'nid' => NULL,
      'title' => $maternal_parent->stock->name
    );
    if (isset($maternal_parent->nid)) {
      $return['nid'] = $maternal_parent->nid;
      $return['title'] = l($maternal_parent->stock->name, 'node/' . $maternal_parent->nid);
    }
  }
  else {
    tripal_report_error(
      'tripal_germplasm',
      TRIPAL_NOTICE,
      'Unable to find the maternal parent for %name (%stock_id)',
      array('%name' => $node->stock->name, '%stock_id' => $node->stock->stock_id)
    );
    return FALSE;
  }

}

/**
 * Retrieve the paternal parent for the given node.
 *
 * @param $node
 *   The node object of the child we want the paternal parent for.
 * @return
 *   An array containing the stock_id, nid, name and title of the paternal
 *   parent where the title is a link to the parent if possible.
 */
function tripal_germplasm_node_part_get_paternal_parent($node) {
  $return = array();

  $paternal_parent = tripal_get_germplasm_parent('paternal',$node->stock);
  if (is_object($paternal_parent)) {

    $return = array(
      'stock_id' => $paternal_parent->stock->stock_id,
      'name' => $paternal_parent->stock->name,
      'nid' => NULL,
      'title' => $paternal_parent->stock->name
    );
    if (isset($paternal_parent->nid)) {
      $return['nid'] = $paternal_parent->nid;
      $return['title'] = l($paternal_parent->stock->name, 'node/' . $paternal_parent->nid);
    }
  }
  else {
    tripal_report_error(
      'tripal_germplasm',
      TRIPAL_NOTICE,
      'Unable to find the paternal parent for %name (%stock_id)',
      array('%name' => $node->stock->name, '%stock_id' => $node->stock->stock_id)
    );
    return FALSE;
  }

}

/**
 * Retrieve the original cross number for the current germplasm node.
 * @param $node
 *   The node object of the germplasm we want the original cross number for.
 * @return
 *   An array containing the stock_id, nid, name and title of the original
 *   cross where the title is a link to the cross if possible.
 */
function tripal_germplasm_node_part_get_original_cross_number($node) {

  $rel = chado_generate_var(
    'stock_relationship',
    array(
      'type_id' => array('name' => 'is_selection_of'),
      'subject_id' => $node->stock->stock_id
    ),
    array(
      'include_fk' => array('object_id' => TRUE)
    )
  );
  if (isset($rel->object_id)) {

    $return = array(
      'stock_id' => $$rel->object_id->stock->stock_id,
      'name' => $$rel->object_id->stock->name,
      'nid' => NULL,
      'title' => $$rel->object_id->stock->name
    );
    if (isset($$rel->object_id->nid)) {
      $return['nid'] = $$rel->object_id->nid;
      $return['title'] = l($$rel->object_id->stock->name, 'node/' . $$rel->object_id->nid);
    }
  }
  else {
    tripal_report_error(
      'tripal_germplasm',
      TRIPAL_NOTICE,
      'Unable to find the original cross for %name (%stock_id)',
      array('%name' => $node->stock->name, '%stock_id' => $node->stock->stock_id)
    );
    return FALSE;
  }
}