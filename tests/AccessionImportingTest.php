<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use StatonLab\TripalTestSuite\Database\Factory;


class AccessionImportingTest extends TripalTestCase {
  use DBTransaction;
  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testForm() {
    $this->assertTrue(true);


  }


  public function testRun() {
    $this->assertTrue(true);


  }

  /**
  * Test with empty db, check for all insertions
  * need genus, species, propter db name selected, accession_number not existed
  */
  public function testFileLoading() {
    module_load_include('inc', 'tripal_germplasm_importer', 'includes/TripalImporter/GermplasmAccessionImporter');
    $this -> insert_test_file();

    // the list for testing, including chado:talbe.column and column order in file
    $test_list = array(
      '' => array(),
      '' => array(),

    );

    //



  }

  /**
  * prepare all variables we need for test
  * using test file to insert some germplams corsses
  * use factory::create to generate organism
  */
  public function insert_test_file(){
    $faker = Factory::create();

    $file_path = DRUPAL_ROOT . '/' . drupal_get_path('module','tripal_germplasm_importer') . '/tests/test_files/';
    //alternative way: $arguments['file'][0]['file_path'] = __DIR__ . '/test_files/unittest_sample_file.tsv';
    $tf = fopen($file_path);
    $all_genus = array();
    $all_species = array();
    $all_dbname = array();
    while ($line_test = fgets($tf)){
      $test_exp = explode('\t', $line_test);
      array_push($all_genus, $test_exp[]);
      array_push($all_species, $test_exp[]);
      array_push($all_dbname, $test_exp[]);
    }
    $organism_genus = array_unique($all_genus); //should be only 1
    $all_species = array_unique($all_species); //should be >= 1
    $all_dbname = array_unique($all_dbname); // should be >= 1
    // need to get genus/species from file, and insert to chado:organism.genus+species
    // also internal database to chado:db.name
    foreach($all_species as $spscies_4create){
      $organism = factory('chado.organism')->create([
        'genus' => $organism_genus,
        'species' => $spscies_4create
      ]);
    }
    foreach($all_dbname as $db_name){
      $db = factory('chado.db') -> create([
        'name' => $db_name
      ]);
    }

    $importer = new \GermplasmAccessionImporter();

    $results = $importer->loadGermplasmAccession($file_path, $organism_genus, $dbxref_id = NULL, $description = NULL, $is_obsolete = f);

    return $results;
  }
}
