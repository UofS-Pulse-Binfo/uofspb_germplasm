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
    // Population Entry.
    // # FIELDSET: Population Entry fieldset.
    $form['fieldset_population_entry'] = [
      '#type' => 'fieldset',
      '#title' => t('Population Entry'),
      '#weight' => -2000
    ];
      
      # FIELD: Autocomplete search.
      // Search germplasm/stock as the population entry.
      $form['fieldset_population_entry']['fld_text_autocomplet_germplasm'] = [
        '#type' => 'textfield',   
        '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/stock',
        '#attributes' => array(
          'placeholder' => t('Germplasm / Stock Name')
        ),
        '#id' => 'population-importer-fld-text-autocomplet-germplasm'
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
        ['cv_id' => ['name' => 'stock_relationship']], 
        ['order by' => ['name' => 'asc']]
      );
      
      $fld_relationship_options = [];
      // This is the default option.
      $fld_relationship_options[0] = '- Select -';

      foreach($stock_relationship as $rel) {
        // Cvterm id as key and name as value. Cvterm id is returned
        // when selection is made.
        $fld_relationship_options[$rel->cvterm_id] = $rel->name;
      }

      $form['fieldset_relationship_verb']['fld_select_relationship_verb'] = [
        '#type' => 'select',
        '#default_value' => 0,
        '#options' => $fld_relationship_options
      ];

      # FIELD: Radio buttons.
      // Set stock position in the relationship.
      $form['fieldset_relationship_verb']['fld_radio_position'] = [
        '#type' => 'radios',
        '#default_value' => 1,
        '#options' => [
          1 => t('Population Entry as SUBJECT and Population Individuals as OBJECT of the relationship.'),
          2 => t('Population Individuals as SUBJECT and Population Entry as OBJECT of the relationship.')
        ]
      ];
      
      # IMAGE: Population load illustration.
      $path = $GLOBALS['base_url'] . '/' . drupal_get_path('module', 'uofspb_germplasm');
      $form['fieldset_relationship_verb']['image_illustration'] = [
        '#markup' => '<div style="margin-top: 20px"><img src="' . $path . '/theme/images/population-load-combination.gif" /></div>'
      ];

    // Population Individuals.
    // # FIELDSET: Population Individuals fieldset.
    // This is Tripal Importer.
    // Default fieldset title of the importer is File Upload, 
    // Replacement to match illustration showing as Population Individuals in declaration.
    
    
    // Submit button.
    // # FIELD: Submit
    // This button is provided by Tripal Importer.









    /*
    $form['info'] = [
      '#type' => 'item',
      '#markup' => 'This importer will create individuals of a population and relate them back to the population stock. More specifically, for every line in the file, a new chado stock record with that information will be created. Then a relationship as specified in this form will be made between that new stock record and the population stock selected in this form. As such this importer can be used in any case where you want to create a number of new stock records related to an existing stock. Examples of such situations are recombinant inbred line populations or nested association mapping panels.',
      '#weight' => -100,
    ];

    $form['population_stock'] = [
      '#title' => 'Population Stock Name',
      '#description' => 'This is the grouping stock record for your population. For example, if this is a RIL, you may select RIL-123 here. This record must already exist.',
      '#type' => 'textfield',
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/stock',
      '#weight' => -49,
    ];

    $form['relationship_type'] = [
      '#title' => 'Relationship Type',
      '#description' => 'This is the type of relationship to connect the grouping population stock record and the new individual stock records from the file.',
      '#type' => 'textfield',
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/',
      '#weight' => -48,
    ];
    */
    
    
    return $form;
  }

  /**
  * @see TripalImporter::run()
  */
  public function run() {
    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['files'][0]['file_path'];

  }

} //end of this class
