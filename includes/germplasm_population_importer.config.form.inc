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
          'placeholder' => t('Germplasm / Stock Name'),
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
      // Verb - cvterm used (under the cv configuration).
      $fld_value_verb = $form_state['values']['fld_select_verb'];
      // Stock population entry: in ABC [id: Stock Id] Notation.
      $fld_value_entry = $form_state['values']['fld_autocomplete_entry'];
      preg_match('/^.*\[id:\s{1}(\d+)\]$/', $fld_value_entry, $match);
      $fld_value_entry = $match[1];
      // Stock position of entry in the relation ship. Object or Subject?
      // pes - population entry is subject and peo - population entry is object.
      $fld_value_position = $form_state['values']['fld_select_position'];
      
      // Fetch stocks that are from the other side of the relationship
      // population individuals.
      $fld1 = ($fld_value_position == 'pes') ? 'object_id'  : 'subject_id';
      $fld2 = ($fld_value_position == 'pes') ? 'subject_id' : 'object_id';
      
      $query = chado_query("
        SELECT stock_id, name, uniquename, description, CONCAT(o.genus, ' ', o.species) AS organism
        FROM {stock} LEFT JOIN {organism} AS o USING(organism_id) 
        WHERE stock_id IN (SELECT $fld1 FROM {stock_relationship} WHERE type_id = :verb AND $fld2 = :entry)
        ORDER BY name ASC 
      ", [':verb' => $fld_value_verb, ':entry' => $fld_value_entry]);
     
      $rows = [];
      if ($query->rowCount() > 0) {
        foreach($query as $i => $individual) {
          $rows[ $i ] = [($i+1), $individual->name, $individual->uniquename, $individual->description, $individual->organism];
        }       
      }

      $header = ['#', 'Germplasm/Stock Name', 'Uniquename', 'Description', 'Scientific Name'];
      $individuals_table = theme('table', ['sticky' => FALSE, 'header' => $header, 'rows' => $rows, 'empty' => '0 germplasm found']);
      $form['fieldset_population_viewer']['summary_table'] = [
        '#type' => 'markup',
        '#markup' => '<div style="height: 200px; overflow-y: scroll">' . $individuals_table . '</div>',
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