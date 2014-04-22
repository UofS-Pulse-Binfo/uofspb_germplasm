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
 * Determine whether a given germplasm is a cross
 *
 * @param $type
 *   An array where the key is either type_id or name
 *   (ie: array('type_id' => 552) or array('name' => 'Backcross'))
 * @return
 *   TRUE if the provided type has been indicated as a cross; FALSE otherwise
 */
function tripal_is_germplasm_cross($type) {
  return TRUE;
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
  return TRUE;
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
  return TRUE;
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
