<?php

/**
 * Get options from the germplasm type controlled vocabulary
 */
function tripal_get_germplasm_type_select_options() {

  $germplasm_type_cv_id = variable_get('germplasm_type_cv_id', 0);
  if ($germplasm_type_cv_id > 0) {
    $options = tripal_get_cvterm_select_options($germplasm_type_cv_id);
  }
  else {
    $options = tripal_get_cvterm_default_select_options('stock','type_id', 'Germplasm Type');
  }

  return $options;
}

/**
 * Get options from the germplasm relationship type controlled vocabulary
 */
function tripal_get_germplasm_relationship_type_cv_id() {

  $germplasm_rel_type_cv_id = variable_get('germplasm_rel_type_cv_id', 0);
  if ($germplasm_rel_type_cv_id > 0) {
    return $germplasm_rel_type_cv_id;
  }
  else {
    return tripal_get_default_cv('stock_relationship','type_id');
  }

}

/**
 * Retrieve a stock object that is the parent of the current stock
 *
 * @param $parent
 *   This should be either 'maternal' or 'parernal' depending on which parent you want
 * @param $child_stock
 *   A chado stock object describing the child whose parent you want to retrieve
 *
 * @return
 *   A chado stock object describing the parent of the current in stock
 */
function tripal_get_germplasm_parent($parent = 'maternal', $child_stock) {

  $parent_rel_type_id = 0;
  if ($parent == 'maternal') {
    $parent_rel_type_id = variable_get('germplasm_mparent_rel_type_id', NULL);
  }
  elseif($parent == 'paternal') {
    $parent_rel_type_id = variable_get('germplasm_pparent_rel_type_id', NULL);
  }

  if ($parent_rel_type_id > 0) {
    $relationships = chado_select_record(
      'stock_relationship',
      array('subject_id'),
      array('object_id' => $child_stock->stock_id, 'type_id' => $parent_rel_type_id)
    );
    if (!empty($relationships)) {
      $stock = chado_generate_var(
        'stock',
        array('stock_id' => $relationships[0]->subject_id)
      );
      $stock = chado_expand_var($stock, 'node', 'stock');
      return $stock;
    }
  }
  else {
    tripal_report_error(
      'tripal_germplasm',
      TRIPAL_ERROR,
      'The %parent relationship type_id has not been set. It can be set at <a href=@url>Admin » Tripal » Extension Modules » Germplasm » Settings</a>.',
      array('%parent' => $parent, '@url' => url('admin/tripal/extension/tripal_germplasm/configuration'))
    );
  }
}

/**
 * Determine whether a given germplasm is a cross
 *
 * @param $type
 *   An array where the key is either type_id or name
 *   (ie: array('type_id' => 552) or array('name' => 'Backcross'))
 * @return
 *   TRUE if the provided type has been indicated as a cross; FALSE otherwise
 */
function tripal_is_germplasm_cross($type) {

  $cross_types = variable_get('germplasm_cross_type_ids', array());
  if (!empty($cross_types)) { $cross_types = unserialize($cross_types); }
  
  // If we have the type_id supplied then just compare directly to the admin selection
  if (isset($type['type_id'])) {
    if (in_array($type['type_id'], $cross_types)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  // Otherwise we need to get the type_id based on the type name
  // and then compare that to the admin selection
  elseif (isset($type['name'])) {
    $type_id = chado_select_record('cvterm', array('cvterm_id'), array('name' => $type['name']));
    $type_id = (!empty($type_id)) ? $type_id[0]->cvterm_id : NULL;

    if (in_array($type_id, $cross_types)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  
  return FALSE;
}

/**
 * Determine whether a given germplasm is a variety
 *
 * @param $type
 *   An array where the key is either type_id or name
 *   (ie: array('type_id' => 552) or array('name' => 'Variety'))
 * @return
 *   TRUE if the provided type has been indicated as a variety; FALSE otherwise
 */
function tripal_is_germplasm_variety($type) {

  $variety_types = variable_get('germplasm_variety_type_ids', array());
  if (!empty($variety_types)) { $variety_types = unserialize($variety_types); }
  
  // If we have the type_id supplied then just compare directly to the admin selection
  if (isset($type['type_id'])) {
    if (in_array($type['type_id'], $variety_types)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  // Otherwise we need to get the type_id based on the type name
  // and then compare that to the admin selection
  elseif (isset($type['name'])) {
    $type_id = chado_select_record('cvterm', array('cvterm_id'), array('name' => $type['name']));
    $type_id = (!empty($type_id)) ? $type_id[0]->cvterm_id : NULL;

    if (in_array($type_id, $variety_types)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  
  return FALSE;
}

/**
 * Determine whether a given germplasm is a RIL
 *
 * @param $type
 *   An array where the key is either type_id or name
 *   (ie: array('type_id' => 552) or array('name' => 'RIL'))
 * @return
 *   TRUE if the provided type has been indicated as a RIL; FALSE otherwise
 */
function tripal_is_germplasm_ril($type) {

  $ril_types = variable_get('germplasm_ril_type_ids', array());
  if (!empty($ril_types)) { $ril_types = unserialize($ril_types); }
  
  // If we have the type_id supplied then just compare directly to the admin selection
  if (isset($type['type_id'])) {
    if (in_array($type['type_id'], $ril_types)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  // Otherwise we need to get the type_id based on the type name
  // and then compare that to the admin selection
  elseif (isset($type['name'])) {
    $type_id = chado_select_record('cvterm', array('cvterm_id'), array('name' => $type['name']));
    $type_id = (!empty($type_id)) ? $type_id[0]->cvterm_id : NULL;

    if (in_array($type_id, $ril_types)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  
  return FALSE;
}

/**
 * Determine whether a given germplasm is a Accession
 *
 * @param $type
 *   An array where the key is either type_id or name
 *   (ie: array('type_id' => 552) or array('name' => 'Accession'))
 * @return
 *   TRUE if the provided type has been indicated as a accession; FALSE otherwise
 */
function tripal_is_germplasm_accession($type) {
  return TRUE;
}
