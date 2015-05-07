<?php

/**
 * @file
 */

/**
 *
 */
function tripal_germplasm_germplasm_node_categories() {
  $categories = array();

  $categories['cross'] = array(
    'title' => 'Cross',
    'check_function' => 'tripal_is_germplasm_cross'
  );

  $categories['variety'] = array(
    'title' => 'Variety',
    'check_function' => 'tripal_is_germplasm_variety'
  );

  $categories['ril'] = array(
    'title' => 'RIL',
    'check_function' => 'tripal_is_germplasm_ril'
  );

  return $categories;
}

/**
 * Define information about each special germplasm node part.
 *
 * Use this hook to define properties, dbxrefs and relationships that you
 * would like special form elements to be added for. This is meant to make the
 * stock form more friendly for entering germplasm.
 *
 * NOTE: Key MUST BE the name of the type.
 */
function tripal_germplasm_special_germplasm_node_fields() {
  $fields = array();

  // Relationships
  //--------------------
  $type_id = tripal_get_germplasm_parent_type_id('maternal');
  $type = tripal_get_cvterm(array('cvterm_id' => $type_id));
  $type_name = 'maternal_parent';
  if (isset($type->cvterm_id)) {
    $type_name = $type->name;
  }
  $fields[$type_name] = array(
    'node_api_category' => 'relationship',
    'title' => 'Maternal Parent',
    'description' => 'The name of the maternal parent of this germplasm.',
    'form_type' => 'textfield',
    'type_name' => $type_name,
    'type_id' => $type_id,
    'current_stock_side' => 'object',
    'category' => array('parent'),
    'type_specific' => FALSE,
    'value_function' => 'tripal_germplasm_node_part_get_maternal_parent'
  );

  $type_id = tripal_get_germplasm_parent_type_id('paternal');
  $type = tripal_get_cvterm(array('cvterm_id' => $type_id));
  $type_name = 'paternal_parent';
  if (isset($type->cvterm_id)) {
    $type_name = $type->name;
  }
  $fields[$type_name] = array(
    'node_api_category' => 'relationship',
    'title' => 'Paternal Parent',
    'description' => 'The name of the paternal parent of this germplasm.',
    'form_type' => 'textfield',
    'type_name' => $type_name,
    'type_id' => $type_id,
    'current_stock_side' => 'object',
    'category' => array('parent'),
    'type_specific' => FALSE,
    'value_function' => 'tripal_germplasm_node_part_get_paternal_parent'
  );

  $type = tripal_get_cvterm(array('name' => 'is_selection_of'));
  $type_id = NULL;
  if (isset($type->cvterm_id)) {
    $type_id = $type->cvterm_id;
  }
  $fields['is_selection_of'] = array(
    'node_api_category' => 'relationship',
    'title' => 'Original Cross Number',
    'description' => 'The cross number the RIL originated from.',
    'form_type' => 'textfield',
    'type_name' => 'is_selection_of',
    'type_id' => $type_id,
    'current_stock_side' => 'subject',
    'category' => array('ril'),
    'type_specific' => TRUE,
    'value_function' => 'tripal_germplasm_node_part_get_original_cross_number'
  );

  // Properties
  //--------------------
  $type = tripal_get_cvterm(array('name' => 'variety_year_released'));
  $type_id = NULL;
  if (isset($type->cvterm_id)) {
    $type_id = $type->cvterm_id;
  }
  $fields['date_registered'] = array(
    'node_api_category' => 'property',
    'title' => 'Date Registered',
    'description' => 'The date this variety was registered.',
    'form_type' => 'textfield',
    'type_id' => $type_id,
    'type_name' => 'variety_year_released',
    'category' => array('variety'),
    'type_specific' => TRUE,
    'value_function' => 'tripal_germplasm_node_part_get_property_value'
  );

  $type = tripal_get_cvterm(array('name' => 'market_class'));
  $type_id = NULL;
  if (isset($type->cvterm_id)) {
    $type_id = $type->cvterm_id;
  }
  $fields['market_class'] = array(
    'node_api_category' => 'property',
    'title' => 'Market Class',
    'description' => 'The market class of this germplasm.',
    'form_type' => 'textfield',
    'type_name' => 'market_class',
    'type_id' => $type_id,
    'category' => array('variety', 'ril'),
    'type_specific' => TRUE,
    'value_function' => 'tripal_germplasm_node_part_get_property_value'
  );

  $type = tripal_get_cvterm(array('name' => 'crossingblock_year'));
  $type_id = NULL;
  if (isset($type->cvterm_id)) {
    $type_id = $type->cvterm_id;
  }
  $fields['crossingblock_year'] = array(
    'node_api_category' => 'property',
    'title' => 'Crossing Block Year',
    'description' => 'The year this cross was made in (ie: '.date("Y").').',
    'form_type' => 'textfield',
    'type_name' => 'crossingblock_year',
    'type_id' => $type_id,
    'category' => array('cross'),
    'type_specific' => TRUE,
    'value_function' => 'tripal_germplasm_node_part_get_property_value'
  );

  $type = tripal_get_cvterm(array('name' => 'crossingblock_season'));
  $type_id = NULL;
  if (isset($type->cvterm_id)) {
    $type_id = $type->cvterm_id;
  }
  $fields['crossingblock_season'] = array(
    'node_api_category' => 'property',
    'title' => 'Crossing Block Season',
    'description' => 'The season this cross was made in (ie: Spring).',
    'form_type' => 'textfield',
    'type_name' => 'crossingblock_season',
    'type_id' => $type_id,
    'category' => array('cross'),
    'type_specific' => TRUE,
    'value_function' => 'tripal_germplasm_node_part_get_property_value'
  );
  return $fields;
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
function tripal_germplasm_node_part_get_maternal_parent($node, $details) {
  $return = array('value'=>'');

  $maternal_parent = tripal_get_germplasm_parent('maternal',$node->stock);
  if (is_object($maternal_parent)) {

    $return = array(
      'stock_id' => $maternal_parent->stock->stock_id,
      'name' => $maternal_parent->stock->name,
      'nid' => NULL,
      'title' => $maternal_parent->stock->name,
      'value' => '(' . $maternal_parent->stock->stock_id . ') ' . $maternal_parent->stock->name
    );
    if (isset($maternal_parent->nid)) {
      $return['nid'] = $maternal_parent->nid;
      $return['title'] = l($maternal_parent->stock->name, 'node/' . $maternal_parent->nid);
    }
  }

  return $return;

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
function tripal_germplasm_node_part_get_paternal_parent($node, $details) {
  $return = array('value'=>'');

  $paternal_parent = tripal_get_germplasm_parent('paternal',$node->stock);
  if (is_object($paternal_parent)) {

    $return = array(
      'stock_id' => $paternal_parent->stock->stock_id,
      'name' => $paternal_parent->stock->name,
      'nid' => NULL,
      'title' => $paternal_parent->stock->name,
      'value' => '(' . $paternal_parent->stock->stock_id . ') ' . $paternal_parent->stock->name
    );
    if (isset($paternal_parent->nid)) {
      $return['nid'] = $paternal_parent->nid;
      $return['title'] = l($paternal_parent->stock->name, 'node/' . $paternal_parent->nid);
    }
  }

  return $return;

}

/**
 * Retrieve the original cross number for the current germplasm node.
 * @param $node
 *   The node object of the germplasm we want the original cross number for.
 * @return
 *   An array containing the stock_id, nid, name and title of the original
 *   cross where the title is a link to the cross if possible.
 */
function tripal_germplasm_node_part_get_original_cross_number($node, $details) {
  $return = array('value'=>'');

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
      'stock_id' => $rel->object_id->stock_id,
      'name' => $rel->object_id->name,
      'nid' => NULL,
      'title' => $rel->object_id->name,
      'value' => '(' .  $rel->object_id->stock_id . ') ' . $rel->object_id->name,
    );
    if (isset($rel->object_id->nid)) {
      $return['nid'] = $rel->object_id->nid;
      $return['title'] = l($rel->object_id->name, 'node/' . $rel->object_id->nid);
    }
  }

  return $return;

}

/**
 * Retrieve the value of the property specified by $details.
 *
 * ASSUMPTION #1: There is only one property of this type.
 *
 * @param $node
 *   The node object of the germplasm we want the original cross number for.
 * @return
 *   An array containing the stockprop_id, value and rank of the specified property.
 */
function tripal_germplasm_node_part_get_property_value($node, $details) {

  $return = array('value'=>'');

  if (isset($node->stock->stockprop)) {
dpm($node->stock->stockprop, 'stockprop');

    foreach($node->stock->stockprop as $prop) {

      if ($prop->type_id->cvterm_id == $details['type_id']) {
        $return = array(
          'stockprop_id' => $prop->stockprop_id,
          'value' => $prop->value,
          'rank' => $prop->rank
        );
      }
    }
  }

  return $return;
}