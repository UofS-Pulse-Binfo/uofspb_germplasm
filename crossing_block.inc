<?php

/**
 * Implements hook_form()
 * @purpose: this form sets the crossing block using the SESSION
 */
function set_crossing_block_form($form_state) {
  $form = array();

    if (!empty($_SESSION['season'])) { $default = $_SESSION['season']; }
    else { $default = '---'; }
    $form['crossblock']['season'] = array( 
      '#type' => 'select', 
      '#title' => t('Season'),
      '#options' => array(
                 '---' => '---',
                 'Spring' => 'Spring',
                 'Summer' => 'Summer',
                 'Fall' => 'Fall',  
                 'Winter' => 'Winter'
        ),
      '#default_value' => $default,
      '#required'   => TRUE
    );

    $current_year = (int)date('Y');
    $years_range = range(1972, $current_year+5, 1); 
    foreach ($years_range as $k => $v) { $years_opt[$v] = $v; } 
    if (!empty($_SESSION['year'])) { $default = $_SESSION['year']; }
    else { $default = $current_year; }
    $form['crossblock']['year'] = array(
      '#type' => 'select',       
      '#title' => t('Year'),   
      '#options' => $years_opt,
      '#default_value' => $default,
      '#required'   => TRUE
    );  

    $stock_organism_options = get_chado_organism_options();
    $stock_organism_options[0] = 'Select An Organism';
    unset($stock_organism_options[array_search('Arabidopsis', $stock_organism_options)]);
    unset($stock_organism_options[array_search('Medicago', $stock_organism_options)]);
    unset($stock_organism_options[array_search('Soybean', $stock_organism_options)]);
    unset($stock_organism_options[array_search('Runner Bean', $stock_organism_options)]);
    unset($stock_organism_options[array_search('Lotus', $stock_organism_options)]);

    if (!empty($form_state['values']['organism'])) { $organism_default = $form_state['values']['organism']; }
    elseif (!empty($_SESSION['organism'])) { $organism_default = $_SESSION['organism']; }
    else { $organism_default = 0; } 
    $form['crossblock']['organism'] = array(
      '#type' => 'select',
      '#title' => t('Source Organism'),
      '#default_value' => $organism_default,
      '#options' => $stock_organism_options,
      '#required'   => TRUE,
    );  

    $form['crossblock']['message'] = array(
      '#type' => 'item',
      '#value' => t('Please ensure to Set the Organism (by selecting a source organism above and clicking the Set Organism button) before entering any crosses. All of the parents available for selection depend on the organism selected here.')
    );

    $form['crossblock']['submit-set_organism'] = array(
      '#type' => 'submit',
      '#value' => t('Set Organism'),
    );

  return $form;
}

/**
 * Implements hook_form_submit
 * Sets the crossing block using session variables
 */
function set_crossing_block_form_submit($form, $form_state) {

  $_SESSION['year'] = $form_state['values']['year'];
  $_SESSION['season'] = $form_state['values']['season'];
  $_SESSION['organism'] = $form_state['values']['organism'];

}

/**
 * Themes the crossing block form
 */
function theme_set_crossing_block_form($form) {

  unset($form['crossblock']['organism']['#title']);  
  $output = '<fieldset><legend>Set Crossing Block</legend>';
  $output .= drupal_render($form['crossblock']['message']);
  $output .= '<table><tr><td>Crossing Block</td><td>'
  	     .drupal_render($form['crossblock']['year']).'</td><td>'
	     .drupal_render($form['crossblock']['season']).'</td></tr>';
  $output .= '<tr><td>Organism</td><td colspan="2">'
  	     .drupal_render($form['crossblock']['organism']).'</td><td>'
	     .drupal_render($form['crossblock']['submit-set_organism']).'</td></tr>';
  $output .= '</table>';
  $output .= '</fieldset>';

  $output .= drupal_render($form);

  return $output;

}