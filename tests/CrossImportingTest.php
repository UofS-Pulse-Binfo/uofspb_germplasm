<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class CrossImportingTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * TEST: did the importer work?
   *
   * Specifically,
   *  - is insertion to table:stock is achieved
   *  - test if each germplasm is inserted with right uniquename, cvterm
   *
   * @group importers
   * @group cross-importer
   */
  public function testCrossInsertion() {

    $insertion_result = $this->insertTestFileHelper();
    $this->assertNotNull($insertion_result['organism_id'],
      "Test file Helper failed.");

    // build an array for testing cvterms, key is the column number , value is the cvterm in chado:cvterm
    // key is the column number-1 to match array of explode line , value is the expected cvterm for each property
    $cvterm_term_check = array(
      '0' => 'crossingblock_year',
      '1' => 'crossingblock_season',
      '5' => 'additionalType',
      '6' => 'seed_type',
      '7' => 'cotyledon_colour',
      '8' => 'seed_coat_colour',
      '9' => 'comment',
    );

    // test for every germplasm line in test file
    $test_file = fopen(($insertion_result['test_file_path']), 'r');
    while ($line = fgets($test_file)) {

      $line = trim($line);

      if (preg_match("/Year/", $line)){continue;}

      $line_explode = explode("\t", $line);

      // test stock insertion in chado:stock
      $results = chado_select_record(
        'stock',
        ['stock_id', 'uniquename', 'type_id'],
        [
          'name'=> $line_explode[2],
          'organism_id'=> $insertion_result['organism_id']
        ]
      );
      $this->assertEquals(1, count($results),
        "There should only be a single chado.stock record for $line_explode[2].");
      $germ_stock_id = $results[0]->stock_id;

      // test cvterm for germplams: F1
      $result = chado_select_record('cvterm', ['name'],
        ['cvterm_id'=>$results[0]->type_id]);
      $this->assertEquals('F1', $result[0]->name,
        "The type for all crosses imported should be 'F1' but it is not for $line_explode[2].");

      // test prefix of uniquename
      $this->assertEquals('0', strpos($results[0]->uniquename, $insertion_result['prefix']),
        "The prefix, '".$insertion_result['prefix']."', is not in the uniquename: '".$results[0]->uniquename."'.");

      // test properties need to insert to chado:stockprop (use $cvterm_term_check)
      foreach($cvterm_term_check as $key => $value) {
        if ($line_explode[$key]) {
          // First select the type_id for the matching propery.
  	      $stockprop = chado_select_record('stockprop', ['type_id'],
            ['stock_id' => $germ_stock_id, 'value' => $line_explode[$key]]);
          // Now get the cvterm name of the property type selected previously.
  	      $cvterm = chado_select_record('cvterm', ['name'],
            ['cvterm_id' => $stockprop[0]->type_id]);
          $this->assertEquals($value, $cvterm[0]->name,
            "The type ('".$cvterm[0]->name."') of the property with the value '".$line_explode[$key]."' does not match what we expected ('".$value."').");
        }
      }

      // test relationship insertions in chado:stock_relationship
      // column 4, maternal parent
      if ($line_explode[3]) {
        $mom = chado_select_record('stock', ['stock_id'],
          ['name' => $line_explode[3], 'organism_id' => $insertion_result['organism_id'] ]);
        if ($mom){
  	      $rel = chado_select_record('stock_relationship', ['type_id'],
            ['subject_id' => $mom[0]->stock_id, 'object_id' => $germ_stock_id]);
  	      $cvterm = chado_select_record('cvterm', ['name'],
            ['cvterm_id' => $rel[0]->type_id]);
          $this->assertEquals('is_maternal_parent_of', $cvterm[0]->name,
            "The maternal parent, '".$line_explode[3]."', relationship type ('".$cvterm[0]->name."') does not match is_maternal_parent_of.");
        }
      }

      // column 5, paternal parent
      if ($line_explode[4]) {
        $dad = chado_select_record('stock', ['stock_id'],
          ['name'=> $line_explode[4], 'organism_id'=> $insertion_result['organism_id'] ]);
        if ($result){
  	      $rel = chado_select_record('stock_relationship', ['type_id'],
            ['subject_id'=>$dad[0]->stock_id, 'object_id'=>$germ_stock_id]);
  	      $cvterm = chado_select_record('cvterm', ['name'],
            ['cvterm_id'=>$rel[0]->type_id]);
          $this->assertEquals('is_paternal_parent_of', $cvterm[0]->name,
            "The maternal parent, '".$line_explode[4]."', relationship type ('".$cvterm[0]->name."') does not match is_paternal_parent_of.");
        }
      }
    }
    fclose($test_file);
  }

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
    $faker = Factory::create();
    module_load_include('inc', 'kp_germplasm', 'includes/TripalImporter/GermplasmCrossImporter');

    // Determine the parameters.
    $file_path = DRUPAL_ROOT . '/' . drupal_get_path('module','kp_germplasm') . '/tests/test_files/germplasm_cross_test1.tsv';
    $organism = factory('chado.organism')->create([
      'genus' => $faker->unique->word . uniqid(),
    ]);
    $organism_id = $organism->organism_id;
    $user_prefix = 'UnitTest';

    // Now load the test file.
    $importer = new \GermplasmCrossImporter();
    ob_start();
    $result = $importer->loadGermplasmCross(
      $file_path,
      $organism_id,
      'UnitTest',
      NULL,
      NULL,
      'f'
    );
    $output = ob_get_contents();
    ob_end_clean();

    // Return the info we'll need for the tests.
    return [
      'test_file_path' => trim($file_path, '"'),
      'organism_id' => $organism_id,
      'prefix' => $user_prefix,
      'output' => $output,
    ];
  }
}
