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
 * Retrieve the cvtmerm ID of the crossingblock property types
 *
 * @param $part
 *   Either 'year' or 'season depending on the type of crossing block property
 *
 * @return
 *   The cvterm ID of the property requested
 */
function tripal_get_germplasm_crossingblock_type_id($part) {
  if ($part == 'season' OR $part == 'year') {
    $type_name = 'crossingblock_' . $part;
    $result = chado_select_record('cvterm',array('cvterm_id'), array('name' => $type_name));
    if (is_array($result)) {
      $type_id = array_pop($result);
      $type_id = $type_id->cvterm_id;
      return $type_id;
    }
  }
  else {
    return FALSE;
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
 * Get the cvterm_id for the maternal/paternal parent relationship type
 * as set in the Tripal Germplasm settings.
 *
 * @param $parent
 *   This should be either 'maternal' or 'parernal' depending on which parent you want
 *
 * @return
 *   The cvterm_id of the relationship type specified
 */
function tripal_get_germplasm_parent_type_id($parent) {
  $parent_rel_type_id = 0;

  if ($parent == 'maternal') {
    $parent_rel_type_id = variable_get('germplasm_mparent_rel_type_id', NULL);
  }
  elseif($parent == 'paternal') {
    $parent_rel_type_id = variable_get('germplasm_pparent_rel_type_id', NULL);
  }

  if ($parent_rel_type_id > 0) {
    return $parent_rel_type_id;
  }
  else {
    return FALSE;
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


/**
 * Handles autocomplete for germplasm names.
 *
 * This is used repeatedly in the node add/edit form.
 *
 * @param $organism_genus
 *    The genus of the preferred organism. Results from all organisms will still
 *    be listed but the preferred organism results will be displayed first.
 * @param $string
 *    The part of the string already typed in the textfield
 *
 * @ingroup tripal_core
 */
function tripal_germplasm_name_to_id_callback($organism_genus, $string) {
  $matches = array();

  if ($organism_genus == 'ALL') {
    $organism_genus = '';
  }

  // Stores the Drupal db_select() queries used to build the results.
  // Only the first 20 results are returned therefore once that number is
  // reached, no subsequent queries are executed.
  $queries = array();

  // $string is the uniquename.
  // Assume they entered the entire uniquename since it was likely copy/pasted.
  $queries['uniquename'] = db_select('chado.stock', 'c');
  $queries['uniquename']->join('chado.organism', 'o', 'c.organism_id = o.organism_id');
  $queries['uniquename']
    ->fields('c', array('stock_id', 'name', 'uniquename'))
    ->fields('o', array('genus','species'))
    ->condition('c.uniquename', $string, '=');

  // $string is part of the name from the preferred organism.
  $queries['name_preferred'] = db_select('chado.stock', 'c');
  $queries['name_preferred']->join('chado.organism', 'o', 'c.organism_id = o.organism_id');
  $queries['name_preferred']
    ->fields('c', array('stock_id', 'name', 'uniquename'))
    ->fields('o', array('genus','species'))
    ->condition('c.name', '%' . db_like($string) . '%', 'LIKE')
    ->condition('o.genus', $organism_genus, '=')
    ->orderBy('char_length(c.name)','ASC')
    ->orderBy('o.genus','ASC');

  // $string is part of the name of any other organism.
  $queries['name_other'] = db_select('chado.stock', 'c');
  $queries['name_other']->join('chado.organism', 'o', 'c.organism_id = o.organism_id');
  $queries['name_other']
    ->fields('c', array('stock_id', 'name', 'uniquename'))
    ->fields('o', array('genus','species'))
    ->condition('c.name', '%' . db_like($string) . '%', 'LIKE')
    ->condition('o.genus', $organism_genus, '!=')
    ->orderBy('char_length(c.name)','ASC')
    ->orderBy('o.genus','ASC');


  // Now compile results based on the above queries.
  foreach ($queries as $query) {

    $result = $query->execute();
    foreach ($result as $row) {
      if (strlen($row->name) > 50) {
        $key = '('.$row->stock_id.') ' . substr($row->name, 0, 50) . '...';
      }
      else {
        $key = '('.$row->stock_id.') ' . $row->name;
      }
      $matches[$key] = format_string(
        '@genus: @name (@uniquename)',
        array(
          '@genus' => check_plain($row->genus),
          '@name' => check_plain($row->name),
          '@uniquename' => check_plain($row->uniquename),
        )
      );

      // If we already have 20 results then break;
      if (count($matches) > 20) { break; }
    }

    // Need to break the outer loop too.
    if (count($matches) > 20) { break; }
  }

  // return for JS
  drupal_json_output($matches);
}