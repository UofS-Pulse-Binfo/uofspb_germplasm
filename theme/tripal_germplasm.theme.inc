<?php

/**
 * @file
 * Keep track of theme-related functions
 */

/**
 * Alter the theme registry so that it knows about the templates we want to
 * override. While this is automatic for themes, it has to be done manually
 * for every template overridden in a module :(.
 *
 * Simply add the name of the template to the $templates_to_override array
 * below and place the new template in [MODULE DIR]/theme.
 */
function tripal_germplasm_theme_registry_alter(&$theme_registry) {

  $module_path = drupal_get_path('module', 'tripal_germplasm');

  // Tell the theme registry to use:
  $templates_to_override = array(
    'tripal_stock_base', 'tripal_stock_relationships'
  );
  foreach ($templates_to_override as $template_name) {
    $theme_registry[$template_name]['theme path'] = $module_path;
    $theme_registry[$template_name]['path'] = $module_path . '/theme/templates';
  }


}

/**
 * Preprocess: tripal_stock_base.tpl.php
 */
function tripal_germplasm_preprocess_tripal_stock_base (&$vars) {

  // Expand the properties.
  $vars['node']->stock = chado_expand_var($vars['node']->stock, 'table','stockprop');

  // Process them so they're easier to list.
  $vars['properties'] = array();
  $vars['synonyms'] = array();
  if (isset($vars['node']->stock->stockprop)) {
    foreach($vars['node']->stock->stockprop as $prop) {
      if ($prop->type_id->name == 'synonym') {
        $vars['synonyms'][] = $prop->value;
      }
      else {
        if (empty($prop->value)) {
          $vars['properties'][ $prop->type_id->name ][] = 'Yes';
        }
        else {
          $vars['properties'][ $prop->type_id->name ][] = $prop->value;
        }
      }
    }
  }
}

/**
 * Preprocess: tripal_stock_relationships.tpl.php
 */
function tripal_germplasm_preprocess_tripal_stock_relationships (&$vars) {

  // Add parental relationship type information.
  foreach(array('maternal', 'paternal') as $parent_type) {
    $type_id = tripal_get_germplasm_parent_type_id($parent_type);
    if ($type_id) {
      $vars['parent_rel_type'][$parent_type] = chado_select_record('cvterm',array('*'), array('cvterm_id' => $type_id));
      $vars['parent_rel_type'][$parent_type] = $vars['parent_rel_type'][$parent_type][0];

      $vars['parent_rel_type'][$parent_type]->title = str_replace('_',' ', $vars['parent_rel_type'][$parent_type]->name);
    }
  }
}