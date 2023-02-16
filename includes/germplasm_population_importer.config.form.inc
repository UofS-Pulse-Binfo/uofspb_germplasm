<?php

/**
 * Function callback - create Germplasm Population Importer configuration form.
 */
function germplasm_population_importer_config_form($form, &$form_state) {
  // Configuration name, set during install/enable to 0.
  $config_verb = 'germplasm_population_importer_verb_cv';
  // Default/current value of the configuration.
  $default_verb = variable_get($config_verb); 
  
  # FIELDSET: Population Viewer.
  $form['fieldset_population_viewer'] = [
    '#type' => 'fieldset',
    '#title' => t('View Population'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
    
    # FIELD: Select term used as the Relationship Verb based.
    // The terms are under the CV configuration above.
    $stock_relationship = chado_select_record('cvterm', 
      ['cvterm_id',  'UPPER(REPLACE(name, \'_\', \' \')) AS name'], 
      ['cv_id' => ['cv_id' => $default_verb]], 
      ['order by' => ['name' => 'asc']]
    );
    
    $fld_verb_options = [];
    // This is the default option.
    $fld_relationship_options[0] = '- Select -';

    foreach($stock_relationship as $rel) {
      // Cvterm id as key and name as value. Cvterm id is returned
      // when selection is made.
      $fld_verb_options[ $rel->cvterm_id ] = $rel->name;
    }

    $form['fieldset_population_viewer']['fld_select_verb'] = [
      '#type' => 'select',
      '#title' => t('Relationship Verb'),
      '#options' => $fld_verb_options,
    ];
    
    # FIELD: Autocomplete Population Entry.
    $form['fieldset_population_viewer']['fld_autocomplete_entry'] = [
      '#type' => 'textfield',
      '#title' => t('Population Entry'),
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/stock',
      '#attributes' => array(
          'placeholder' => t('Germplasm / Stock Name')
      ),
    ];

    # FIELD: Select the stock position.
    $form['fieldset_population_viewer']['fld_select_position'] = [
      '#type' => 'select',
      '#title' => t('Stock Position'),
      '#options' => [
        'pes' => 'Population Entry is the Subject',
        'peo' => 'Population Entry is the Object',
      ],
    ];
    
    # FIELD: Submit button. 
    // Fetch the other side of the Relationship.
    $form['fieldset_population_viewer']['fld_button_submit'] = [
      '#type' => 'submit',
      '#value' => 'Load Population Individuals',
      '#submit' => ['germplasm_population_load_individuals'],
    ]; 
   
    // Construct population individuals summary table of germplasm.
    if (isset($form_state['values'])) {
      $individuals_table = theme('table', ['sticky' => FALSE, 'header' => ['Germplasm/Stock Name'], 'rows' => [['a' => 1]]]);
      
      $form['fieldset_population_viewer']['summary_table'] = [
        '#type' => 'markup',
        '#markup' => $individuals_table
      ];

      $form['fieldset_population_viewer']['#collapsed'] = FALSE;
    }

  # FIELDSET: configuration fieldset.
  $form['fieldset_importer_configuration'] = [
    '#type' => 'fieldset',
    '#title' => t('Configure Germplasm Population Importer'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  ];
  
    # FIELD: Germplasm Population Importer verb controlled vocabulary setting.
    // Create a select option showing all cvs currently available.
    // Set it to default value of the configuration variable.
    $cvs = chado_select_record('cv', ['cv_id', 'name'], [], ['order by' => ['name' => 'asc']]);
    $fld_cv_options = [];
    $fld_cv_options[0] = '- Select -';

    foreach($cvs as $cv) {
      $fld_cv_options[ $cv->cv_id ] = $cv->name;
    }
    
    $form['fieldset_importer_configuration'][ $config_verb ] = [
      '#type' => 'select',
      '#options' => $fld_cv_options,
      '#default_value' => $default_verb,
      '#description' => t('Germplasm Population Importer uses term in a Controlled Vocabulary (CV) 
        as the Relationship Verb (ie. stock_relationship). Set this field to the controlled 
        vocabulary containing the terms that represent relationship verb.'),
    ];
  
    # FIELD: Germplasm name default prefix.
    // Configuration name, set during install/enable to GERM.
    $config_prefix = 'germplasm_population_importer_default_prefix';
    // Default/current value of the configuration.
    $default_prefix = variable_get($config_prefix);

    $form['fieldset_importer_configuration'][ $config_prefix ] = [
      '#type' => 'textfield',
      '#default_value' => $default_prefix,
      '#description' => t('Use this field value as default germplasm prefix.'),
    ];

  return system_settings_form($form);
}

/**
 * Population viewer function callback.
 */
function germplasm_population_load_individuals($form, &$form_state) {
  // Capture form input and prepare items (population individuals) table.
  $form_state['rebuild'] = TRUE;
}