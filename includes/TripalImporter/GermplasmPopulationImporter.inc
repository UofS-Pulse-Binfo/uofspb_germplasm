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
  public static $upload_description = 'Germplasm file should be a tab separated file (.tsv) containing a header with the following colums:<ol>
      <li><strong>Name</strong>: The name of the RIL individual.</li>
      <li><strong>Type</strong>: The vocabulary database name + : + cvterm (ie. schema:F1) which should be used for the stock record, must exist.</li>
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
      $form['fieldset_population_entry']['fld_text_population_entry'] = [
        '#type' => 'textfield',   
        '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/stock',
        '#attributes' => array(
          'placeholder' => t('Germplasm / Stock Name')
        ),
        '#disabled' => $disabled,
        '#required' => TRUE,
        '#id' => 'population-importer-fld-text-population-entry'
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
      // evi: entry-verb-individual.
      // ive: individual-verb-entry.
      $form['fieldset_relationship_verb']['fld_radio_stock_position'] = [
        '#type' => 'radios',
        '#default_value' => 'evi',
        '#disabled' => $disabled,
        '#options' => [
          'evi' => t('Population Entry as SUBJECT and Population Individuals as OBJECT of the relationship.'),
          'ive' => t('Population Individuals as SUBJECT and Population Entry as OBJECT of the relationship.')
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
    $fld_name_population_entry = 'fld_text_population_entry';
    $fld_value_population_entry = $form_state['values'][ $fld_name_population_entry ];
    
    // Gerplasm Autocomplete Notation: ABC 123 [id: 123]:
    $stock = $this->parseStock($fld_value_population_entry);
    if (!stock) {
      form_set_error($fld_name_population_entry, t('Germplasm does not exist. Please enter a valid germplasm in Population Entry.'));
    }

    # Validate Relationship Verb.
    // 0 - Select: is selected for this field.
    $fld_name_relationship_verb = 'fld_select_relationship_verb';
    $fld_value_relationship_verb = $form_state['values'][ $fld_name_relationship_verb ];
    if ($fld_value_relationship_verb == 0) {
      // None selected in relationship verb select field.
      form_set_error($fld_name_relationship_verb, t('Please select a value in Relationship Verb.'));
    }

    # Validate Prefix (If provided with).
    // No more than 20 characters for prefix.
    $fld_name_prefix = 'fld_text_stock_prefix';
    $fld_value_prefix = $form_state['values'][ $fld_name_prefix ];
    if (!empty($fld_value_prefix) && strlen($fld_value_prefix) > 20) {
      form_set_error($fld_name_prefix, t('Prefix Error. Please ensure prefix is no more than 20 characters.'));
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
              // Append each error encountered with the line number starting
              // with the header as line 1.
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
                // Each line, Type (db cv:cvterm) is checked to ensure that it exists in chado.cvterm. In the same
                // way, organism is checked using the genus + species derived from Scientific Name.
                $type = $this->parseTerm($val_type);
                if (!$type) {
                  form_set_error(0, $error_message . 'Type does not exist.');
                }

                $organism = $this->parseOrganism($val_sciname);
                if (!$organism) {
                  form_set_error(0, $error_message . 'Organism does not exist.');
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
                
                // Stock Name+Type+Scientific Name combination must be unique.
                $result = chado_select_record('stock', ['stock_id'], [
                  'name' => $val_name,
                  'type_id' => $type,
                  'organism_id' => $organism
                ]);
                  
                if ($result) {
                  form_set_error(0, $error_message . 'Name+Type+Scientific Name combination is already used by another germplasm.');
                }
                
              }
            }
            
            // Next line.
            $i++;
          }

          fclose($handle);
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
    // All values provided by the user in the Importer's form widgets are
    // made available to us here by the Class' arguments member variable.
    $arguments = $this->arguments['run_args'];
    
    // Prefix when not provided will default to prefix 
    // system configuration.
    $configuration = $this->getConfigurationSettings();
    $prefix = ($arguments['fld_text_stock_prefix']) 
      ? $arguments['fld_text_stock_prefix'] 
      : $configuration['prefix']; 
    
    // Isolate population entry stock id given the format
    // Stock Name [id: Stock ID Number].
    $population_entry_stock_id = $this->parseStock($arguments['fld_text_population_entry']);

    // Form values.
    $population = [
      'entry' =>  $population_entry_stock_id,                  // Stock Id Number.
      'verb'    => $arguments['fld_select_relationship_verb'], // Cvterm (Type) Id Numnber.
      'position'  => $arguments['fld_radio_stock_position'],   // EVI or IVE position type.
      'individuals' => $this->arguments['files'][0]['fid'],    // Individuals File Id.
      'prefix' => $prefix                                      // Default Prefix.
    ];

    $this->importPopulation($population);
  }

  /**
   * Function callback, create population.
   * 
   * @param $population
   *   Array, with the following keys:
   *   entry: Form field value for Population Entry field.
   *   verb : Form field value for Relationship Verb field.
   *   position: Form field value for Stock Position field.
   *   individuals: Form file field value for Population Individuals Field.
   *   prefix: Form field value for Prefix field.
   */
  public function importPopulation(array $population) {
    // 1) Recognize that the loader is not hung if running a large file, but is executing
    // 2) Provides some indication for how long the file will take to load.
    
    // Here we'll get the size of the file and tell the TripalImporter how
    // many "items" we have to process (in this case bytes of the file).
    $file = file_load($population['individuals']);
    $this->setTotalItems($file->filesize);
    $this->setItemsHandled(0);
    
    $TRANSACTION = db_transaction();
    try {
      if ($file && $file->filesize > 0) {
        $file_path = drupal_realpath($file->uri);
        $handle = fopen($file_path, 'r');
        if ($handle) {
          $i = 0;
          $bytes_read = 0;

          // Fetch the last stock_id auto-increment inserted, increment
          // the value each time a stock is added. This value is concatenated
          // to the prefix (when provided) that will make up the uniquename of the stock.
          // Skip this when file has provided a custom uniquename.
          $id = chado_select_record('stock', ['stock_id'], [], ['order by' => ['stock_id' => 'DESC'], 'limit' => 1]);
          $last_id = $id[0]->stock_id;

          while(!feof($handle)) {
            // Add all individuals in file into chado.stock table
            // and simultaneously creating the Relationship verb.
            $cur_line = fgets($handle);      
            $i++;
      
            if ($i > 0 && trim($cur_line) != '') {
              // Calculate how many bytes we have read from the file and let the
              // importer know how many have been processed so it can provide a
              // progress indicator.
              $bytes_read += drupal_strlen($cur_line);
              $this->setItemsHandled($bytes_read);

              $values = str_getcsv($cur_line, "\t");
              // Skip the header row.
              @list($val_name, $val_type, $val_sciname, $val_uniqname) = array_map('trim', $values);
     
              # Construct uniquename:
              // If line has no uniquename by using the prefix system configuration
              // and next sequence id of stock.
              $uniquename = ($val_uniqname == '') 
                ? $population['prefix'] . ':' . ($last_id + $i) : $val_uniqname;
              // Always encode in uppercase form.
              $uniquename = strtoupper($uniquename);

              # Description:
              // Tag the stock with brief description of the source plus a timestamp.
              $description = 'This germplasm was added through the Germplasm Population Importer. ' 
                . date('l jS \of F Y h:i:s A');  

              # Organism:
              // Use the genus and species encoded in the Scientific Name to determine the organism.
              $organism_id = $this->parseOrganism($val_sciname);

              # Type:
              // Use the database name and cvterm encoded in Type to determine the stock type.
              $type_id = $this->parseTerm($val_type);
              
              // Population individual to insert.
              $stock = [
                'name'      => $val_name,
                'uniquename'  => $uniquename,
                'organism_id'  => $organism_id,
                'description' => $description,
                'type_id'   => $type_id,
              ];

              // Save the id of the individual being added 
              // and use it in the relationship below. 
              $individual = chado_insert_record('stock', $stock);
              
              # Create Relationship:
              $relation = [];
              // Verb.
              $relation['type_id'] = $population['verb'];
                
              // Position.
              if ($population['position'] == 'evi') {
                // Entry - Verb - Individual.
                $relation['subject_id'] = $population['entry'];
                $relation['object_id'] = $individual['stock_id'];
              }
              elseif ($population['position'] == 'ive') {
                // Individual - Verb - Entry
                $relation['object_id'] = $individual['stock_id'];
                $relation['subject_id']  = $population['entry'];
              }

              chado_insert_record('stock_relationship', $relation);
              unset($individual);
            }
          }

          fclose($handle);
          exit(0);
        }
      }
    }
    catch(Exception $e) {
      $TRANSACTION->rollback();
      exit(1);
    }
  }

  /**
   * Get configuration settings.
   *
   * @return array
   *   Assoc. array with keys verb and prefix containing the 
   *   configuration value of verb and prefix, respectively.
   */
  public function getConfigurationSettings() {
    // Relationship verb configuruation.
    $verb_configuration = variable_get('germplasm_population_importer_verb_cv');
    // Prefix configuration.
    $prefix_configuration = variable_get('germplasm_population_importer_default_prefix');

    return [
      'verb' => $verb_configuration, 
      'prefix' => $prefix_configuration
    ];
  }

  /**
   * Parse form values for Stock/Germplasm (Population Entry) and fetch
   * the matching row in chado.stock table.
   * 
   * @param $value
   *   String, containing the stock name and stock id in 
   *   the following notation: Stock Name\s[id:\sStock Id Number].
   *
   * @return integer
   *   Stock id number that matched the resolved stock id 
   *   from the input string.
   */
  public function parseStock($value) {
    $result = '';
    $table  = 'stock';
    
    // Capture the stock id sequence of numbers in brackets.
    preg_match('/^.*\[id:\s{1}(\d+)\]$/', $value, $match);
    if ($match !== FALSE && $match[1] > 0) {
      $values = [
        'stock_id' => (int) $match[1],
      ];
      
      // Fetch stock that match the stock id enclosed in
      // the input string form value.
      $select = chado_select_record($table, ['stock_id'], $values);
      if ($select) {
        $result = $select[0]->stock_id;
      }
    }
    
    return $result;
  }
  
  /**
   * Parse form values for cv/dbname:cvterm name (Type) and fetch
   * the matching row in chado.cvterm. ie. SO:F1
   * 
   * @param $value
   *   String, containing the database name and cvterm name in 
   *   the following notation: CV/Database name:Cvterm name.
   * 
   * @return integer
   *   Cvterm id number that matched the resolved cvterm id
   *   from the input string.
   */ 
  public function parseTerm($value) {
    $result = '';
    $table  = 'cvterm';

    // Capture the database name and cvterm name.
    preg_match('/^(.+):{1}(.+)$/', $value, $match);
    if ($match !== FALSE && ($match[1] && $match[2])) {
      $values = [
        'name' => $match[2],
        'dbxref_id' => ['db_id' => ['name' => $match[1]]],
      ];

      // Fetch cvterm that match the dbname and cvterm name
      // in the input string form value. The dbname will
      // ensure that a specific term will be returned.
      $select = chado_select_record($table, ['cvterm_id'], $values);
      if ($select) {
        $result = $select[0]->cvterm_id;
      }
    }

    return $result;
  }
 
  /**
   * Parse form values for Scientific Name (ogranism: genus+species) and
   * fetch the matching row in chado.organism.
   * 
   * @param $value
   *   String, containing the genus and species in the
   *   following notation: Genus\sSpecies. ie. Lens culinaris.
   *
   */   
  public function parseOrganism($value) {
    $result = '';
    $table  = 'organism';

    // Capture the genus and species from Scientific Name value.
    preg_match('/^(\w+)\s{1}(.*)/', $value, $match);
    if ($match !== FALSE && ($match[1] && $match[2])) {
      $values = [
        'genus' => $match[1],
        'species' => $match[2],
      ];

      // Fetch organism using the genus+species and return
      // the organism_id number.
      $select = chado_select_record('organism', ['organism_id'], $values);
      if ($select) {
        $result = $select[0]->organism_id;
      }
    }

    return $result;
  }
} //end of this class