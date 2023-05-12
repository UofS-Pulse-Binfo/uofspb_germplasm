<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\DatabaseSeeders\GermplasmPopulationSeeder;

class GermplasmPopulationImportingTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testGermplasmPopulationImporter() {
    // Is module enabled:
    $is_enabled = module_exists('uofspb_germplasm');
    $this->assertTrue($is_enabled, 'Module is enabled');

    // Preapre data from database seeder.
    $seeder = new GermplasmPopulationSeeder();
    $data = $seeder->up();

    // Importer instance.
    module_load_include('inc', 'uofspb_germplasm', 'includes/TripalImporter/GermplasmPopulationImporter');
    $population_importer = new \GermplasmPopulationImporter();
    
    // CONFIGURATIONS:
    // After install. By default, the module will set the default verb to 0,  default db to 0 and prefix to GERM.
    $configurations = $population_importer->getConfigurationSettings();
    $this->assertEquals($configurations['verb'], 0, 'Test default verb must be 0 on module install.');
    $this->assertEquals($configurations['prefix'], 'GERM', 'Test default prefix configuration does not match the term GERM.');
    $this->assertEquals($configurations['db'], 0, 'Test default db must be 0 on module install.');
    

    // Set the default verb to a term:
    // Prepare to use the importer.
    // @see seeder class.
    variable_set('germplasm_population_importer_verb_cv', $data['cv_verb_id']);
    variable_set('germplasm_population_importer_db', $data['database']);

    $configurations = $population_importer->getConfigurationSettings();
    $this->assertEquals($configurations['verb'], $data['cv_verb_id'], 'Test verb cv must match database entry.');
    $this->assertEquals($configurations['db'], $data['database'], 'Test database must must match database entry.');
    $this->assertEquals($configurations['prefix'], 'GERM', 'Test default prefix configuration does not match the term GERM.');

    // PARSE GERMPLASM NAME:
    // Test parse stock/germplasm method given germplasm where the notation 
    // is Germplasm Name [id: Id Number] as value returned by autocomplete germplasm field.
    // This is: Stock Sbj 001 [id: Stock ID]:
    $stock_name = array_keys($data['stock_subject'])[0];
    $stock_id = array_values($data['stock_subject'])[0];
    $autocomplete_name = $stock_name . ' [id: ' . $stock_id . ']';
    $result_stock_id = $population_importer->parseStock($autocomplete_name);
    
    $this->assertEquals($stock_id, $result_stock_id, 'Test parse germplasm id failed to match record stock id.');

    // Test other variation of germplasm names.
    foreach($data['stocks'] as $name => $id) {
      $autocomplete_name = $name . ' [id: ' . $id . ']';
      $result_stock_id = $population_importer->parseStock($autocomplete_name);

      $this->assertEquals($id, $result_stock_id, 'Test parse germplasm id failed to match record stock id');
    }
    
    // PARSE TYPE (dbname:cvterm name) IN FILE:
    // Test parse TYPE column in file for cvterm id as type given a value in a format
    // database name:cvterm name.
    $db = $data['cv_terms'];
    foreach($data['type_terms'] as $name => $id) {
      $autocomplete_term = $db . ':' . $name;
      $result_term_id = $population_importer->parseTerm($autocomplete_term);

      $this->assertEquals($id, $result_term_id, 'Test parse Type (cvterm id) failed to match record cvterm.');
    }
    
    // PARSE SCIENTIFIC NAME (Organism - Genus + Species) IN FILE:
    // Test parse SCIENTIFIC NAME in file for ogranism as scientific name gicen a value
    // in a format Genus species.
    $scientific_name = $data['organism']['genus'] . ' ' . $data['organism']['species'];
    $result_organism_id = $population_importer->parseOrganism($scientific_name);

    $this->assertEquals($data['organism_id'], $result_organism_id, 'Test parse Scientific Name failed to match record organism.');
  
    $file_path = DRUPAL_ROOT . '/' . drupal_get_path('module','uofspb_germplasm');
    
    // IMPORTER:
    // Test 1: 
    // Population entry is the subject.
    // Use prefix configuration - GERM.
    // @see test_files/germplasm_population_test1.tsv.
    $f =  $file_path . '/tests/test_files/germplasm_population_test1.tsv';
    $file = file_save((object)[
      'filename' => 'germplasm_population_test1.tsv',
      'uri' => $f
    ]);
    
    $population = [
      'entry' => $data['stock_subject']['Stock Sbj 001'],
      'verb' => $data['term_verb_id'],
      'position' => 'evi', // Entry Verb Indv.
      'individuals' => $file->fid,
      'prefix' => $configurations['prefix'],
      'db' => $configurations['db'],
    ];

    $population_importer->importPopulation($population);

    // Stocks inserted should match the file.
    $file_path = drupal_realpath($file->uri);
    $handle = fopen($file_path, 'r');
    $i = 0;
    $stocks_inserted = [];
    while($cur_line = fgets($handle)) {
      if ($i == 0) {
        // This is the header row.
        $i++;
        continue;
      }

      $values = str_getcsv($cur_line, "\t");
      @list($val_name, $val_type, $val_sciname, $val_uniqname) = array_map('trim', $values);
      $type_id = $population_importer->parseTerm($val_type);
      $organism_id = $population_importer->parseOrganism($val_sciname);

      $germ = chado_select_record('stock', ['stock_id', 'name', 'uniquename', 'dbxref_id'], [
        'name' => $val_name,
        'type_id' => $type_id,
        'organism_id' => $organism_id
      ]);

      // Stock:
      // Assert stock inserted name matched.
      $this->assertEquals($germ[0]->name, $val_name, 'Stock inserted name does not match name in file.');
      // Uniquename matches expected uniquename generated using default prefix configuration.
      $this->assertEquals($germ[0]->uniquename, $configurations['prefix'] . $germ[0]->stock_id, 
        'Stock inserted uniquename is not in the expected value.');
    
      $stocks_inserted[ $germ[0]->stock_id ] = $germ[0]->stock_id;

      // Dbxref:
      // Dbxref.db_id matches the db_id configuration and accession is the uniquename generated.
      $dbxref = chado_select_record('dbxref', ['db_id', 'accession'], ['dbxref_id' => $germ[0]->dbxref_id]);
      $this->assertEquals($germ[0]->uniquename, $dbxref[0]->accession, 'Stock inserted dbxref does not match a record in chado.dbxref.');
      $this->assertEquals($configurations['db'], $dbxref[0]->db_id, 'Stock inserted dbxref.db_id does not match a record in chado.db.');        
    }

    fclose($handle);

    // Relations:
    $rels = chado_select_record('stock_relationship', ['subject_id', 'object_id'], [
      'subject_id' => $population['entry'],
      'type_id' => $population['verb']
    ]);

    // Assert that it created a number of relations equal to number of items in the file.
    $this->assertEquals(count($rels), count($stocks_inserted), 'Number of stock relations should match the number of items in the file.');
    // Assert that item in the file should have a relation of an entry + stock.
    foreach($rels as $i => $rel) {
      $this->assertEquals($rel->subject_id, $population['entry'], 'Unexpected stock relation created.');
      $this->assertEquals($rel->object_id, $stocks_inserted[ $rel->object_id ], 'Unexpected stock relation created.');
    }
  
    // Test 2: 
    // Population entry is the object.
    // Use custom uniquename entered in the file for each stock.
    // @see test_files/germplasm_population_test2.tsv.
    $f =  $file_path . '/tests/test_files/germplasm_population_test2.tsv';
    $file = file_save((object)[
      'filename' => 'germplasm_population_test2.tsv',
      'uri' => $f
    ]);
    
    $population = [
      'entry' => $data['stock_subject']['Stock Obj 002'],
      'verb' => $data['term_verb_id'],
      'position' => 'ive', // Indv. Verb Entry.
      'individuals' => $file->fid,
      'prefix' => $configurations['prefix'],
      'db' => $configurations['db'],
    ];

    $population_importer->importPopulation($population);

    // Stocks inserted should match the file.
    $file_path = drupal_realpath($file->uri);
    $handle = fopen($file_path, 'r');
    $i = 0;
    $stocks_inserted = [];
    while($cur_line = fgets($handle)) {
      if ($i == 0) {
        // This is the header row.
        $i++;
        continue;
      }

      $values = str_getcsv($cur_line, "\t");
      @list($val_name, $val_type, $val_sciname, $val_uniqname) = array_map('trim', $values);
      $type_id = $population_importer->parseTerm($val_type);
      $organism_id = $population_importer->parseOrganism($val_sciname);

      $germ = chado_select_record('stock', ['stock_id', 'name', 'uniquename', 'dbxref_id'], [
        'name' => $val_name,
        'type_id' => $type_id,
        'organism_id' => $organism_id
      ]);

      // Stock:
      // Assert stock inserted name matched.
      $this->assertEquals($germ[0]->name, $val_name, 'Stock inserted name does not match name in file.');
      // Uniquename matches expected uniquename generated using default prefix configuration.
      $this->assertEquals($germ[0]->uniquename, $val_uniqname, 'Stock inserted uniquename is not in the expected value.');
    
      $stocks_inserted[ $germ[0]->stock_id ] = $germ[0]->stock_id;

      // Dbxref:
      // Dbxref.db_id matches the db_id configuration and accession is the uniquename generated.
      $dbxref = chado_select_record('dbxref', ['db_id', 'accession'], ['dbxref_id' => $germ[0]->dbxref_id]);
      $this->assertEquals($germ[0]->uniquename, $dbxref[0]->accession, 'Stock inserted dbxref does not match a record in chado.dbxref.');
      $this->assertEquals($configurations['db'], $dbxref[0]->db_id, 'Stock inserted dbxref.db_id does not match a record in chado.db.');
    }

    fclose($handle);

    // Relations:
    $rels = chado_select_record('stock_relationship', ['subject_id', 'object_id'], [
      'object_id' => $population['entry'],
      'type_id' => $population['verb']
    ]);

    // Assert that it created a number of relations equal to number of items in the file.
    $this->assertEquals(count($rels), count($stocks_inserted), 'Number of stock relations should match the number of items in the file.');
    // Assert that item in the file should have a relation of an entry + stock.
    foreach($rels as $i => $rel) {
      $this->assertEquals($rel->object_id, $population['entry'], 'Unexpected stock relation created.');
      $this->assertEquals($rel->subject_id, $stocks_inserted[ $rel->subject_id ], 'Unexpected stock relation created.');
    }
  }
}
