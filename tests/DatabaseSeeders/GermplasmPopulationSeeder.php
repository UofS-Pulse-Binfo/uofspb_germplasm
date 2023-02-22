<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;
use \Exception;

class GermplasmPopulationSeeder extends Seeder {
  public function up() {
    $data = [];

    // Add cv - stock_relationship.
    // This will be used for relationship verb configuration.
    $cv_sr = 'stock_relationship';
    $cv = chado_insert_cv($cv_sr, 'CV stock relationship');
    $data['cv_verb'] = $cv_sr;
    $data['cv_verb_id'] = $cv->cv_id;
    
    $terms = [
      'is_population', // Use in test as relationship verb.
      'is_germplasm',
      'is_stock', 
    ];

    foreach ($terms as $i => $term) {
      $id = chado_insert_cvterm([
        'id' => $cv_sr . $term,
        'name' => $term,
        'definition' => $term . ' definition',
        'cv_name' => $cv_sr,
        'db_name' => $cv_sr,
      ]);
      
      if ($i == 0) {
        // Save individual of population term.
        $data['term_verb_id'] = $id->cvterm_id;
      }
    }

    // Add cv - my_cvterms
    // This will be used to test method that
    // extracts cvterms from Type column (in file).
    $cv_my = 'my_cvterms';
    chado_insert_cv($cv_my, 'CV terms variations');
    $data['cv_terms'] = $cv_my;

    $terms = [
      'my term 1', // Use as stock type below.
      'my_term_2', // Use as value for Type column in file.
      'myTerm3',
      'my-term-4',
      'myterm5',
      'my.term.6',
      'myterm7',
      'My Term 8',
      'MY TERM 9',
      'my term (10)'
    ];

    $stock_type = 0;
    foreach ($terms as $i => $term) {
      $id = chado_insert_cvterm([
        'id' => $term,
        'name' => $term,
        'definition' => $term . ' definition',
        'cv_name' => $cv_my,
        'db_name' => $cv_my,
      ]);
      
      $data['type_terms'][ $term ] = $id->cvterm_id;

      if ($i == 0) {
        // Save my term 2.
        $stock_type = $id->cvterm_id;
      }
    }
    
    // Insert stocks.
    // Organism:
    $data['organism'] = ['genus' => 'Tripalus', 'species' => 'databasica (abc)'];
    $organism = chado_insert_record('organism', array(
      'abbreviation' => 'T. databasica',
      'genus' => $data['organism']['genus'],
      'species' => $data['organism']['species'],
      'common_name' => 'Cultivated Tripal'
    ));

    $data['organism_id'] = $organism['organism_id'];
    
    // Stock as subject.
    $stock = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock Sbj 001',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $stock_type,
      'dbxref_id' => null,
      'description' => 'This stock will be used as subject',
    ));
    
    // Use in test as population entry being the subject.
    $data['stock_subject'] = ['Stock Sbj 001' => $stock['stock_id']];
    unset($stock);

    // Stock as object.
    $stock = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock Obj 002',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $stock_type,
      'dbxref_id' => null,
      'description' => 'This stock will be used as object',
    ));
    
    // Use in test as population entry being the object.
    $data['stock_object'] = ['Stock Obj 002' => $stock['stock_id']];

    // Stock name variations:
    $stocks = ['GERM 111', 'ABC Germ 1', '54-BA', '3974-A', 'AABB', 'NoNAME GERMPLASM (Duplicate)'];
    foreach($stocks as $stock) {
      $s = chado_insert_record('stock', array(
        'organism_id' => $organism['organism_id'],
        'name' => $stock,
        'uniquename' => 'uname-' . uniqid(),
        'type_id' => $stock_type,
        'dbxref_id' => null,
        'description' => 'This is a random stock name format',
      ));

      $data['stocks'][ $stock ] = $s['stock_id'];
    }

    return $data;
  }
}