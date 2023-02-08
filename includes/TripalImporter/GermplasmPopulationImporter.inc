<?php

/**
 * @see TripalImporter
 */
 class GermplasmPopulationImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Germplasm Population Importer';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_germplasm_population_importer';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Imports germplasm populations (i.e. RIL, NAM, cross progeny) into chado.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['tsv', 'txt'];

  /**
   * If the loader should require an analysis record.  To maintain provenance
   * we should always indicate where the data we are uploading comes from.
   * The method that Tripal attempts to use for this by associating upload files
   * with an analysis record.  The analysis record provides the details for
   * how the file was created or obtained. Set this to FALSE if the loader
   * should not require an analysis when loading. if $use_analysis is set to
   * true then the form values will have an 'analysis_id' key in the $form_state
   * array on submitted forms.
   */
  public static $use_analysis = FALSE;

  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   */
  public static $require_analysis = FALSE;

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Germplasm file should be a tab separated file containing a header with the following colums:<ol>
      <li><strong>Name</strong>: The name of the RIL individual.</li>
      <li><strong>Type</strong>: The name of the cvterm which should be used for the stock record, must exist.</li>
      <li><strong>Scientific Name</strong>: The genus + species of the organism to be used for the stock record, must exist.</li>
      <li><strong>Uniquename</strong>: The uniquename to use if you do not want to use the pattern/prefix below.</li>
    </ol>
    
    <p>Each row in the file should describe a specific individual to be created and linked to the Population Entry with the specified relationship.</p>
  ';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = array(
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  );

  // Alters default impoter title to match population load combination illustrations.
  public static $upload_title = 'Population Individuals';

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    // Enable all field elements to work and let configration 
    // check below decide should fields be disabled.
    $disabled = FALSE;

    # INFO:
    $form['info'] = [
      '#type' => 'item',
      '#weight' => -3000,
      '#markup' => t('This importer will create individuals of a population and 
        relate them back to the population stock. More specifically, for every line 
        in the file, a new chado stock record with that information will be created. 
        Then a relationship as specified in this form will be made between that new 
        stock record and the population stock selected in this form. As such this 
        importer can be used in any case where you want to create a number of new 
        stock records related to an existing stock. Examples of such situations are 
        recombinant inbred line populations or nested association mapping panels.')
    ];

    // Do allow importer if not configured.
    $configuration = $this->getConfigurationSettings();
    if ($configuration['verb'] == 0) {
      $l = l(t('Configure Importer'), 'admin/tripal/extension/germplasm_population_importer/configure');
      $form['not_configured'] = [
        '#markup' => '<div class="messages error">
          This germplasm population importer is not configured correctly. ' . $l . '</div>',
        '#weight' => -2500  
      ];

      $disabled = TRUE;
    }  

    // Population Entry.
    // # FIELDSET: Population Entry fieldset.
    $form['fieldset_population_entry'] = [
      '#type' => 'fieldset',
      '#title' => t('Population Entry'),
      '#weight' => -2000
    ];
      
      # FIELD: Autocomplete search.
      // Search germplasm/stock as the population entry.
      $form['fieldset_population_entry']['fld_text_autocomplete_germplasm'] = [
        '#type' => 'textfield',   
        '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/stock',
        '#required' => TRUE,
        '#attributes' => array(
          'placeholder' => t('Germplasm / Stock Name')
        ),
        '#disabled' => $disabled,
        '#required' => TRUE,
        '#id' => 'population-importer-fld-text-autocomplete-germplasm'
      ];


    // Relationship Verb.
    // # FIELDSET: Relationship Verb fieldset.
    $form['fieldset_relationship_verb'] = [
      '#type' => 'fieldset',
      '#title' => t('Relationship Verb'),
      '#weight' => -1000
    ];

      # FIELD: Select relationship verb.
      // Query all terms under cv = stock_relationship, text process replacing
      // all underscore with spaces and capitalized to improve readability. 
      // Cvterm id is returned.
      $stock_relationship = chado_select_record('cvterm', 
        ['cvterm_id',  'UPPER(REPLACE(name, \'_\', \' \')) AS name'], 
        ['cv_id' => ['cv_id' => $configuration['verb']]], 
        ['order by' => ['name' => 'asc']]
      );
      
      $fld_relationship_options = [];
      // This is the default option.
      $fld_relationship_options[0] = '- Select -';

      foreach($stock_relationship as $rel) {
        // Cvterm id as key and name as value. Cvterm id is returned
        // when selection is made.
        $fld_relationship_options[ $rel->cvterm_id ] = $rel->name;
      }

      $form['fieldset_relationship_verb']['fld_select_relationship_verb'] = [
        '#type' => 'select',
        '#default_value' => 0,
        '#options' => $fld_relationship_options,
        '#disabled' => $disabled,
        '#attributes' => ['style' => ['width: 449px']],
        '#required' => TRUE,
        '#id' => 'population-importer-fld-select-relationship-verb'
      ];

      # FIELD: Radio buttons.
      // Set stock position in the relationship.
      $form['fieldset_relationship_verb']['fld_radio_position'] = [
        '#type' => 'radios',
        '#default_value' => 1,
        '#disabled' => $disabled,
        '#options' => [
          1 => t('Population Entry as SUBJECT and Population Individuals as OBJECT of the relationship.'),
          2 => t('Population Individuals as SUBJECT and Population Entry as OBJECT of the relationship.')
        ]
      ];
      
      # IMAGE: Population load illustration.
      $path = $GLOBALS['base_url'] . '/' . drupal_get_path('module', 'uofspb_germplasm');
      $form['fieldset_relationship_verb']['image_illustration'] = [
        '#markup' => '<div style="margin-top: 20px"><img src="' . $path . '/theme/images/population-load-combination.gif" style="max-width: 70%" /></div>'
      ];

    // Population Individuals.
    // # FIELDSET: Population Individuals fieldset.
    // This is Tripal Importer.
    // Default fieldset title of the importer is File Upload, 
    // Replacement to match illustration showing as Population Individuals in declaration.
    
    # FIELD: Text field.
    // Allow user to specify which prefix to use in naming germplasm uniquename.
    // @see hook_form_alter().
    
    // Submit button.
    // # FIELD: Submit
    // This button is provided by Tripal Importer.
    
    return $form;
  }

  
  /**
   * Validate form field values.
   */
  public function formValidate($form, &$form_state) {
    // Always call the TripalImporter (i.e. parent) formValidate as it provides
    // some important feature needed to make the form work properly.
    parent::formValidate($form, $form_state);

    # Validate Population Entry.
    $fld_name_population_entry = 'fld_text_autocomplete_germplasm';
    $fld_value_population_entry = $form_state['values'][ $fld_name_population_entry ];
    
    // Gerplasm Autocomplete Notation: ABC 123 [id: 123]:
    preg_match('/^.* \[id: (\d*)\]$/', $fld_value_population_entry, $match);
    if ($match !== FALSE && $match[1] > 0) {
      // Check if germplasm id do exist.
      $stock_id = $match[1];
      $stock = chado_select_record('stock', ['stock_id'], ['stock_id' => $stock_id]);
      if (!$stock) {
        // Confirmed, germplasm does not exist.
        form_set_error($fld_name_population_entry, t('Germplasm does not exist. Please enter a valid germplasm in Population Entry.'));
      }
    }
    else {
      // Could not establish germplasm id.
      form_set_error($fld_name_population_entry, t('Please enter a valid germplasm in Population Entry.'));
    }

    # Validate Relationship Verb.
    $fld_name_relationship_verb = 'fld_select_relationship_verb';
    $fld_value_relationship_verb = $form_state['values'][ $fld_name_relationship_verb ];
    if ($fld_value_relationship_verb == 0) {
      // None selected in relationship verb select field.
      form_set_error($fld_name_relationship_verb, t('Please select a value in Relationship Verb.'));
    }

    # Validate Prefix (If provided with).
    // No more than 10 characters for prefix.
    $fld_name_prefix = 'fld_text_prefix';
    $fld_value_prefix = $form_state['values'][ $fld_name_prefix ];
    if (!empty($fld_value_prefix) && strlen($fld_value_prefix) > 20) {
      form_set_error($fld_name_prefix, t('Please ensure prefix is no more than 20 characters.'));
    }
    
    # Validate File.
    // File - contents and headers.
    $file_id = $form_state['values']['file_upload'];
    // Each line has name, type, scientific name and uniquename.
    if ($file_id > 0) {
      $file = file_load($file_id);

      if ($file && $file->filesize > 0) {
        $file_path = drupal_realpath($file->uri);
        $handle = fopen($file_path, 'r');

        if ($handle) {
          $i = 0;
          while(!feof($handle)) {
            // Current line.
            $cur_line = fgets($handle);

            if ($i == 0) {
              # VALIDATE HEADER LINE:
              if (trim($cur_line) == '') {
                // Line 0 is not the column header.
                form_set_error(0, 'Could not find column headers row in line #1.');
              }
              else {
                // 0:Name, 1:Type, 2:Scientific Name (genus + species), 3:Uniquename.
                $headers = str_getcsv($cur_line, "\t");
                if (strtolower($headers[0])    != 'name' 
                    || strtolower($headers[1]) != 'type'
                    || strtolower($headers[2]) != 'scientific name'
                    || strtolower($headers[3]) != 'uniquename') {
                      
                  form_set_error(0, 'Invalid column header found. Please review column header requirements.');
                }
              }
            }
            else {
              # VALIDATE DATA ROW:
              $error_message = 'ERROR: Germplasm in line #:' . ($i + 1) . '. ';

              if (trim($cur_line) == '') {
                // A data row is empty.
                form_set_error(0, $error_message . 'Line is an empty row.');
              }
              else {
                // Peform subsequent validation assuming there is a row to check.            
                $values = str_getcsv($cur_line, "\t");
                @list($val_name, $val_type, $val_sciname, $val_uniqname) = array_map('trim', $values);
                
                # BASIC VALIDATION:
                // Each column with the exception of uniquename (which is optional)
                // must have a value. Type and Scientific Name should contain a string value.
                if ($val_name == '' || $val_type == '' || $val_sciname == '') {
                  form_set_error(0, $error_message . 'Column(s) does not have a value.');
                }
                
                if (ctype_digit($val_type) || ctype_digit($val_sciname)) {
                  form_set_error(0, $error_message . 'Invalid numeric value in Type and/or Scientific Name.');
                }


                # TERMS VALIDATION:
                // Each line, Type is checked to ensure that it exists in chado.cvterm. In the same
                // way, organism is checked using the genus + species derived from Scientific Name.
                $result = chado_select_record('cvterm', ['cvterm_id'], ['name' => $val_type]);
                if (!$result) {
                  form_set_error(0, $error_message . 'Type does not exist.');
                }
                $val_term_id = $result['cvterm_id'];

                preg_match('/^(\w+)\s{1}(.*)/', $val_sciname, $match);
                $organism = ['genus' => trim($match[1]), 'species' => trim($match[2])];                
                if ($organism['genus'] && $organism['species']) {
                  $result = chado_select_record('organism', ['organism_id'], $organism);
                  if (!$result) {
                    form_set_error(0, $error_message . 'Organism does not exist.');
                  }
                }


                # GERMPLASM VALIDATION:
                // Every uniquename (when provided) is checked to ensure that there is no copy in chado.stock table.
                // Germplasm+name+Organism combination is unique.
                if ($val_uniqname) {
                  $result = chado_select_record('stock', ['stock_id'], ['uniquename' => $val_uniqname]);
                  if ($result) {
                    form_set_error(0, $error_message . 'Uniquename is already used by another germplasm.');
                  }
                }

                $result = chado_select_record('stock', ['stock_id'], [
                  'name' => $val_name,
                  'type_id' => $val_term_id,
                  'organism_id' => ['genus' => $organism['genus'], 'species' => $organism['species']]
                ]);
                if ($result) {
                  form_set_error(0, $error_message . 'Name+Type+Scientific Name combination is already used by another germplasm.');
                }
              }
            }
            
            // Next line.
            $i++;
          }
        }
      }
      else {
        // Not a valid file or is an empty file.
        form_set_error(0, 'Not a valid file or file is empty.');
      }
    }
  }

  /**
  * @see TripalImporter::run()
  */
  public function run() {
    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['files'][0]['file_path'];

  }

  /**
   * Function callback, create population.
   * 
   * @param $entry
   *   Integer, the stock id number of germplasm as the Population Entry.
   * @param $verb
   *   Integer, the cvterm id number of a term as the Relationship Verb.
   * @param $individuals
   *   File object, file containing germplasm individuals as Population Individuals.
   * @param $prefix
   *   String, prefix used when naming germplasm. Default to GERM.
   */
  public function createPopulation($entry, $verb, $individuals, $prefix) {

  }

  /**
   * Get configuration settings.
   */
  public function getConfigurationSettings() {
    // Relationship verb configuruation.
    $verb_configuration = variable_get('germplasm_population_importer_verb_cv');
    // Prefix configuration.
    $prefix_configuration = variable_get('germplasm_population_importer_default_prefix');

    return ['verb' => $verb_configuration, 'prefix' => $prefix_configuration];
  }
} //end of this class
