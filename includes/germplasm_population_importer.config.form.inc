<?php

/**
 * Function callback - create Germplasm Population Importer configuration form.
 */
function germplasm_population_importer_config_form($form, &$form_state) {
  # FIELDSET: configuration fieldset.
  $form['fieldset_importer_configuration'] = [
    '#type' => 'fieldset',
    '#title' => t('Configure Germplasm Population Importer'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  ];
  
    # FIELD: Germplasm Population Importer verb controlled vocabulary setting.
    // Configuration name, set during install/enable to 0.
    $config_name = 'germplasm_population_importer_verb_cv';
    // Default/current value of the configuration.
    $default_config = variable_get($config_name); 

    // Create a select option showing all cvs currently available.
    // Set it to default value of the configuration variable.
    $cvs = chado_select_record('cv', ['cv_id', 'name'], [], ['order by' => ['name' => 'asc']]);
    $fld_cv_options = [];
    $fld_cv_options[0] = '- Select -';

    foreach($cvs as $cv) {
      $fld_cv_options[ $cv->cv_id ] = $cv->name;
    }

    $form['fieldset_importer_configuration'][ $config_name ] = [
      '#type' => 'select',
      '#options' => $fld_cv_options,
      '#default_value' => $default_config,
      '#description' => t('Germplasm Population Importer uses term in a Controlled Vocabulary (CV) 
        as the Relationship Verb (ie. stock_relationship). Set this field to the controlled 
        vocabulary containing the terms that represent relationship verb.'),
    ];
  

  return system_settings_form($form);
}
