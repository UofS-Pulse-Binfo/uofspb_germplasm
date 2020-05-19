<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use StatonLab\TripalTestSuite\Database\Factory;


class AccessionImportingTest extends TripalTestCase {
  use DBTransaction;

  public static $AccessionInporterColumns = [
    '0' => [
      'table' => 'stock',
      'column' => 'name'
    ],
    '1' => [
      'table' => 'db',
      'column' => 'name'
    ],
    '2' => [
      'table' => 'stock',
      'column' => 'uniquename'
    ],
    '3' => [
      'table' => 'organism',
      'column' => 'genus'
    ],
    '4' => [
      'table' => 'organism',
      'column' => 'species'
    ],
    '5' => [
      'table' => 'organism',
      'column' => 'infraspecific_name'
    ],
    '6' => [
      'table' => 'stockprop',
      'column' => 'value',
      'cvterm' => 'institute code'
    ],
    '7' => [
      'table' => 'stockprop',
      'column' => 'value',
      'cvterm' => 'breeding institute name'
    ],
    '8' => [
      'table' => 'stockprop',
      'column' => 'value',
      'cvterm' => 'country of origin'
    ],
    '9' => [
      'table' => 'stockprop',
      'column' => 'value',
      'cvterm' => 'biological status of accession'
    ],
    '10' => [
      'table' => 'stockprop',
      'column' => 'value',
      'cvterm' => 'method'
    ],
    '11' => [
      'table' => 'stockprop',
      'column' => 'value',
      'cvterm' => 'country of origin'
    ],
    '12' => [
      'table' => 'synonym',
      'column' => 'name'
    ],
  ];
  /**
   * Test with empty db, check for all insertions
   * need genus, species, propter db name selected, accession_number not existed
   *
   * @group importers
   * @group accession-importer
   */
  public function testFileLoading() {

    $insertion_result = $this->insertTestFileHelper();
    $this->assertNotNull($insertion_result['organism']['Tripalus databasica']->organism_id,
      "Test file Helper failed.");

    // @todo actually test the file insertion using the columns mentioned above.

  } //end of this test

  /**
   * HELPER: Loads the test file into the database using the importer.
   *
   * File: test_files/unittest_sample_file.tsv
   *
   * Approach:
   *  - prepare all variables we need for test
   *  - using test file to insert some germplams corsses
   *  - use factory::create to generate organism
   */
  public function insertTestFileHelper(){
    $faker = \Faker\Factory::create();
    module_load_include('inc', 'uofspb_germplasm', 'includes/TripalImporter/GermplasmAccessionImporter');

    // Prepare the parameters to load the test file.
    $file_path = DRUPAL_ROOT . '/' . drupal_get_path('module','uofspb_germplasm') . '/tests/test_files/germplasm_accession_test1.tsv';
    //alternative way: $arguments['file'][0]['file_path'] = __DIR__ . '/test_files/unittest_sample_file.tsv';
    $organism1 = factory('chado.organism')->create([
      'genus' => 'Tripalus',
      'species' => 'databasica',
    ]);
    $organism2 = factory('chado.organism')->create([
      'genus' => 'Tripalus',
      'species' => 'databasica',
    ]);
    $db1 = factory('chado.db') -> create([
      'name' => 'Tripal Breeding',
    ]);
    $db2 = factory('chado.db') -> create([
      'name' => 'Wild Tripal Association',
    ]);

    // Now import the file.
    $importer = new \GermplasmAccessionImporter();
    ob_start();
    $importer->loadGermplasmAccession($file_path, $organism_genus, $dbxref_id = NULL, $description = NULL, $is_obsolete = f);
    $output = ob_get_contents();
    ob_end_clean();

    // Return the results and specific parameters.
    return [
      'organism' => [
        'Tripalus databasica' => $organism1,
        'Tripalus ferox' => $organism2,
      ],
      'database' => [
        'Tripal Breeding' => $db1,
        'Wild Tripal Association' =>$db2,
      ],
      'output' => $output,
    ];
  }
}
