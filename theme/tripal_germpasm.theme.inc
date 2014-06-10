<?php

/**
 * @file
 * Keep track of theme-related functions
 */

function tripal_germplasm_preprocess_tripal_germplasm_crossmanage_workflow(&$variables) {
  $css_file = drupal_get_path('module','tripal_germplasm').'/theme/css/cross_management_forms.css';
  drupal_add_css($css_file);

  if ($path = libraries_get_path('jquery_ui')) {
    drupal_add_css($path . '/css/kptheme/jquery-ui-1.10.4.custom.css');
    drupal_add_js($path . '/js/jquery-1.10.2.js');
    drupal_add_js($path . '/js/jquery-ui-1.10.4.custom.js');
  }
}
