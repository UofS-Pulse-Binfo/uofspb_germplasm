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
      <li>Individual Name: Name of the individual to be added to the germplasm population.</li>
      <li>Type Name: The name of the controlled vocabulary term indicating the type of germplasm (e.g. accession).</li>
      <li>Type Accession: The full accession of the controlled vocabulary term indicating the type of germplasm (e.g. CO_010:0000044).</li>
      <li>Genus: The genus of the individual germplasm.</li>
      <li>Species: The species of the individual germplasm.</li>
      <li>Accession (OPTIONAL): A unique identifier for the individual germplasm.</li>
    </ol>';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = array(
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  );

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

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

    /**
    $form['relationship_type'] = [
      '#title' => 'Relationship Type',
      '#description' => 'This is the type of relationship to connect the grouping populati0on stock record and the new individual stock records from the file.',
      '#type' => 'textfield',
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/stock',
      '#weight' => -48,
    ];
    **/

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
