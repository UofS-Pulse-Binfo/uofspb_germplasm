<?php

/////////////////////////////////////////////////////////////////////////////////
// Pedigree interface
/////////////////////////////////////////////////////////////////////////////////


/**
 * This form is the main form for the Germplasm manage pedigree. It takes a organism-id, 
 * stock name and a level to render a pedigree diagram, enabling users to see the lineage
 * traced back to the level ammount
 *
 *  @param &$form_state
 *    The form state that is NULL and can be updated at any point
 *
 */
function germplasm_manage_generate_pedigree_form (&$form_state=NULL) {
  
   //Creating Fieldset for multiple fields in form
  $form['pedigree_render'] = array(
    '#type' => 'fieldset',
    '#title' => t(''),
  );
  
  //selecting 
	$results = tripal_core_chado_select('organism',array('organism_id','common_name'),array());
	
	$organism_options = array(); 
	
	foreach ($results as $r) {
		$organism_options[$r->organism_id] = $r->common_name;
	}
	
	$form['pedigree_render']['organism_id'] = array(
		'#type' => 'select',
		'#title' => t('Source Organism'),
		'#options' => $organism_options,
		'#required'   => TRUE,
		'#default_value' => $form_state['storage']['organism_id'],
	); 
    
  //Stock Name
  $form['pedigree_render']['stock_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Stock'),
    '#description' => t('Stock Name.'),
    '#required'   => TRUE,
		'#default_value' => $form_state['storage']['stock_name'],
  );
  
  $form['pedigree_render']['pedigree_type'] = array(
      '#type' => 'radios', 
      '#title' => t('Find Parents or Children'), 
      '#required'   => TRUE,
      '#options' => array('subject' => t('Parental Pedigree'), 'object' => t('Offspring Pedigree')),
      '#default_value' => $form_state['storage']['pedigree_type'],
  );
  
  //Levels of Prodigney Tree
  $form['pedigree_render']['level'] = array(
    '#type' => 'textfield',
    '#title' => t('Generations'),
    '#options' => $items,
    '#required'   => TRUE,
    '#description' => t('This is the maximum level of generation that will be processed'),
		'#default_value' => (empty($form_state['storage']['level'])) ?3 : $form_state['storage']['level'],
  );
           
  $form['pedigree_render']['submit'] = array(
   '#type' => 'submit', 
   '#weight' => 10,
   '#value' => t('Submit')
   );
   
  if (!empty($form_state['storage']['pedigree'])) {
    $text  = '<div id="tripal_stock-germplasm_pedigree-box" class="germplasm_manage-info-box tripal-info-box">'; 
    $text .= '<div class="germplasm_manage-info-box-title tripal-info-box-title">Pedigree</div>';
     $text  .= '<div class="tripal_germplasm_manage-info-box-desc tripal-info-box-desc">The pedigree that is depicted 
                below is a representation of the lineage or genealogical descent of the selected stock.
                The lineage has been traced to '.$form_state['values']['level'].' generations. </div>';	
     $text .= '<table class="tripal_germplasm_manage-table tripal-table tripal-table-horz">';
     $text .=	'<tr><th>Pedigree</th></tr>';					
     $text	.= '<tr><td class="tripal_germplasm_manage-table-even-row tripal-table-even-row">'.$form_state['storage']['pedigree_string']."</td></tr>";
     $text .=	'</table>';
     $text .= theme('germplasm_manage_pedigree_drawing', $form_state['storage']['pedigree']);
     $text .=	'</div>';
   }
   
   $form['pedigree']=array(
     '#type' => (empty($form_state['storage']['pedigree_string'])) ?'hidden' : 'item',
     '#value' => $text,
   );
     
  return $form;

}


/**
 * This function's purpose is to validate that the user entered the required information
 * and that the information is in the correct format: the stocks entered are valid and present,
 * to many lines of the stock name match more that one stock name 
 *
 *  @param $form
 *    the form that is to be validated
 *
 *  @param &$form_state
 *    The state of the form,containing the user entered information
 */
function germplasm_manage_generate_pedigree_form_validate ($form, &$form_state) {


  $results = tripal_stock_get_stock_by_name_identifier($form_state['values']['stock_name'], $form_state['values']['organism_id']);
  
  if (sizeof($results) > 1) {
      $links= array();       
      for ($i=0; $i<sizeof($results); $i++) { $links[] = l($i+1, "node/".$results[$i]->nid); }
      $message = "Too many lines match '".$form_state['values']['stock_name']."'! "
                 . "Please refine your input to match ONLY ONE line. <br>"              
                 . "To aid in this process, here are the lines that match your initial input: "
                 .join(', ',$links);                                                            
      form_set_error('stock_id', $message);                                                    
  } elseif (sizeof($results) < 1) {                                                    
      form_set_error('stock_id', "There are no stocks matching your input. Please check your input for typos and/or search the germplasm ".l('here', 'stocks'));
  } elseif (sizeof($results) == 1) { 
  		
      $form_state['values']['stock_name'] = $results[0]->stock->name;
      $form_state['values']['stock_id'] = $results[0]->stock->stock_id;
  } 

}


/**
 * Implements hook_form_submit
 *
 *  @param $form
 *    the form that is to be validated
 *
 *  @param &$form_state
 *    The state of the form,containing the user entered information
 */
function germplasm_manage_generate_pedigree_form_submit($form, &$form_state){

	$form_state['storage']['stock_name'] = $form_state['values']['stock_name'];
	$form_state['storage']['stock_id'] = $form_state['values']['stock_id'];
	$form_state['storage']['organism_id'] = $form_state['values']['organism_id'];
	$form_state['storage']['pedigree_type'] = $form_state['values']['pedigree_type'];
	$form_state['storage']['level'] = $form_state['values']['level'];

  if ($form_state['storage']['pedigree_type'] == 'object') {
    $form_state['storage']['pedigree'] = germplasm_manage_generate_object_pedigree_array($form_state['storage']['stock_id'], $form_state['storage']['level']);
  } else {
    $form_state['storage']['pedigree'] = germplasm_manage_generate_subject_pedigree_array($form_state['storage']['stock_id'], $form_state['storage']['level']);
    $form_state['storage']['pedigree_string'] = germplasm_manage_generate_textual_pedigree_string($form_state['storage']['pedigree'], $form_state['storage']['level']);
  }
    
}


////////////////////////////////////////////////////////////////////////////////
// Pedigree Node Form & Form Submit
////////////////////////////////////////////////////////////////////////////////

/**
 * This Form is on the stock node page and provides a form for adding arguments into the path
 * that are then used when the pedigree is generated
 *
 * @return
 *   An array (following conventions of the drupal form API) describing the form
 */
function germplasm_manage_generate_pedigree_on_node_form(){

  if(!empty($_GET['generation'])){
    $default_generation = $_GET['generation'];
 	}else{
 	   $default_generation = 3;
 	}
 	
  if(!empty($_GET['pedigree_type'])){
    $default_pedigreetype = $_GET['pedigree_type'];
 	}else{
 	   $default_pedigreetype = 'subject';
 	}  
  
  //Levels of Prodigney Tree
  $form['level'] = array(
    '#type' => 'textfield',
    //'#title' => t('Generations'),
    '#options' => $items,
    '#required'   => TRUE,
		'#default_value' => $default_generation,
		'#description' => 'The number of generations to display; the number of times to expand to include the '
		  .'relationships of the subject/object in the relationship (ie: if 2 is entered, the not only the '
		  .'relationship Maria [subject] is_maternal_parent_of Fred [object/current stock] is displayed but '
		  .'also the relationship Lukey is_maternal_parent_of Maria and Peppers is_maternal_parent_of Maria).'
  );
  
  $form['familyselect'] = array(
      '#type' => 'radios', 
      //'#title' => t('Find Parents or Children'), 
      '#default_value' => $default_pedigreetype, 
      '#options' => array(
        'subject' => 'Parents', 
        'object' => 'Offspring'
      ),
      '#description' => 'All relationships where the current stock is the object (ie: Maria [subject] '
        .'is_maternal_parent_of Fred [object/current stock]) are returned when Parents is chosen. When '
        .'Offspring is chosen, all relationships where the current stock is the subject are returned '
        .'(ie: Fred [subject/current stock] is_patenral_parent_of Missy [object]).'
  );
  

  $form['submit'] = array(
    '#type' => 'submit', 
    '#weight' => 10,
    '#value' => t('Submit'),
    '#prefix' => '<div class="views-exposed-form">', 
		'#suffix' => '</div>',
   ); 
     
       
  return $form;
}

/**
 * Implements hook_form_submit
 */
function germplasm_manage_generate_pedigree_on_node_form_submit($form, &$form_state){

  $generation_array = array(
    'generation'=> $form_state['values']['level'],
    'block' => 'germplasm_pedigree',
    'pedigree_type'=> $form_state['values']['familyselect'],
  );
  
  //dpm($generation_array,'arrayin form submit');
  
 drupal_goto($_GET['q'], $generation_array);

}

/**
 * Themes the form described in germplasm_manage_generate_pedigree_on_node_form()
 *
 * @param $form
 *   The form as described in germplasm_manage_generate_pedigree_on_node_form()
 * @return
 *   HTML text describing the form
 */
function theme_germplasm_manage_generate_pedigree_on_node_form ($form) {
  $output = '';

  $output .= "<table>\n";
  $output .= "<tr><th>Number of Generations</th><th>Direction to Follow Relationships</th><th></th></tr>\n";
  $output .= "<tr>\n";
  $output .= "<td title='".$form['level']['#description']."'>";
  unset($form['level']['#description']);
  $output .= drupal_render($form['level'])."</td>\n";
  $output .= "<td title='".$form['familyselect']['#description']."'>";
  unset($form['familyselect']['#description']);
  $output .= drupal_render($form['familyselect'])."</td>\n";
  $output .= "<td>".drupal_render($form['submit'])."</td>\n";
  $output .= "</tr>\n";
  $output .= "</table>\n";
  
  $output .= drupal_render($form);
  return $output;
}

////////////////////////////////////////////////////////////////////////////////
// Themeing functions to add javascript code to the germplasm_manage_pedigree.tpl.php
////////////////////////////////////////////////////////////////////////////////

/**
 * This is a theme-ing function that generates the HTML/Javascript
 * needed to display the pedigree drawing
 *
 * Specifically, the passed in pedigree array (PHP) needs to be printed out as a javascript
 * array in order to pass it into the pedigree_tree.js constructor. Also the javascript calls
 * building the tree and the HTML to build the canvas need to be generated.
 *
 * @param $pedigree_array
 *   The PHP form of the rawtree array needed for the pedigree_tree.js constructor
 *   This array is generated by germplasm_manage_generate_pedigree_array()
 * @return
 *   The HTML/Javascript output needed to build the drawing of the pedigree tree
 *   described by the passed in pedigree_array
 *
 */
function theme_germplasm_manage_pedigree_jsarray ($pedigree_array) {
  $output = '';
  
  drupal_add_js(drupal_get_path('theme','tripal').'/germplasm_theme/js/raphael.js', 'module', 'header');
  drupal_add_js(drupal_get_path('theme','tripal').'/germplasm_theme/js/pedigree_tree.js', 'module', 'header');
  
  $output .= germplasm_manage_generate_javascript_pedigree_array($pedigree_array, 'rawtree');
  $output .= "\n";
  
  return $output;
}

/**
 * Returns javascript code encoding the array passed in (Recursive!)
 *
 * @param $pedigree_array
 *   The PHP form of the rawtree array needed for the pedigree_tree.js constructor
 *   This array is generated by germplasm_manage_generate_pedigree_array()
 * @return
 *   Javascript code encoding the array passed in
 */
function germplasm_manage_generate_javascript_pedigree_array ($pedigree_array, $new_array_name) {
  $output = '';

  $output .= "    " . $new_array_name." = new Array();\n";
  foreach ($pedigree_array as $key => $value) {
    if (!is_array($value)) {
      $output .= "    " . $new_array_name."['".$key."'] = '".$value."';\n";    
    } else {
      $expanded_output = germplasm_manage_generate_javascript_pedigree_array($value, $new_array_name."['".$key."']");
      $output .= $expanded_output;  
    }
  }
  
  return $output;
}


////////////////////////////////////////////////////////////////////////////////
// Object Pedigree generation
//
// Generates a pedigree where only relationships where the current stock is the
// subject are used. This is often referred to as a offspring pedigree since
// this module assumes the relationship types is_maternal_parent_of and is_paternal_parent_of
////////////////////////////////////////////////////////////////////////////////

/**
 * Generates an array describing all relationships where the current stock_id is the subject
 *
 * This array can then be used to draw a pedigree diagram
 *
 * @param $current_stock_id
 *   The stock_id which is equal to the subject_id of all stock_relationships to be pulled out
 * @return
 *   A pedigree array describing a number of stock relationships
 */
function germplasm_manage_generate_object_pedigree_array($current_stock_id, $total_levels, $cur_level = 1) {
  global $base_path;
  
  // Get all relationships where current stock_id=subject_id
  $relationships = tripal_core_generate_chado_var(
    'stock_relationship',
    array('subject_id'=>$current_stock_id)
  );
  //dpm($relationships, 'Relationships');
  
  $cur_stock = tripal_core_generate_chado_var('stock',array('stock_id'=>$current_stock_id));
  
  // Base Case: No relationships
  if (empty($relationships)) {
    //dpm('Base Case: '.$cur_stock->name);
    //dpm($cur_stock, 'current stock');
    $new_pedigree = array(
      'stock_id' => $cur_stock->stock_id,
      'name' => $cur_stock->name,
      'nid' => $cur_stock->nid,
      'link' => $base_path.'node/'.$cur_stock->nid,
      'children' => array(),
    );
    if ($cur_level == 1) {
      $new_pedigree = array( 0 => $new_pedigree );
    }
    return $new_pedigree;
  
  // Recursive Case: 1+ Relationships we haven't reached the final level yet
  } elseif ($cur_level < $total_levels) {
    //dpm('1+ Relationships & < total_levels: '.$cur_stock->name);
    $new_pedigree = array(
      'stock_id' => $cur_stock->stock_id,
      'name' => $cur_stock->name,
      'nid' => $cur_stock->nid,
      'link' => $base_path.'node/'.$cur_stock->nid,
      'children' => array(),
    );
    
    // Deal with case where there's only 1 relationship
    if (is_object($relationships)) {
      $relationships = array( 0 => $relationships);
    }
    
    // For each relationship
    foreach ($relationships as $k => $r) {
      $new_pedigree['children'][$k] = germplasm_manage_generate_object_pedigree_array($r->object_id->stock_id, $total_levels, $cur_level + 1);
      $new_pedigree['children'][$k]['relationship_type'] = $r->type_id->name;
    }
    if ($cur_level == 1) {
      $new_pedigree = array( 0 => $new_pedigree );
    }
    return $new_pedigree;
    
  // Case: 1+ Relationships but we Have reached the final level 
  } else {
    //dpm('1+ Relationships & == total_levels: '.$cur_stock->name);
    $new_pedigree = array(
      'stock_id' => $cur_stock->stock_id,
      'name' => $cur_stock->name,
      'nid' => $cur_stock->nid,
      'link' => $base_path.'node/'.$cur_stock->nid,
      'children' => array(),
    );
    return $new_pedigree;

  }
}

////////////////////////////////////////////////////////////////////////////////
// Subject Pedigree generation
//
// Generates a pedigree where only relationships where the current stock is the
// object are used. This is often referred to as a parental pedigree since
// this module assumes the relationship types is_maternal_parent_of and is_paternal_parent_of
////////////////////////////////////////////////////////////////////////////////

/**
 * Generates a pedigree array up to a maximum depth where the current stock is the object 
 * of the relationship. If you have subject is_m/paternal_parent_of object relationships then
 * this function will give you a pedigree array describing the parents of the initial current stock
 *
 * @param $current_stock_id
 *   Stock relationships where the object_id is the current_stock_id
 * @param $total_levels
 *   The maximum depth to travel
 * @param $pedigree
 *   The pedigree array generated so far. This is used for recursive function calls.
 * @return
 *   A pedigree array where the initial current_stock_id is the deepest stock
 *
 */
function germplasm_manage_generate_subject_pedigree_array($current_stock_id, $total_levels, $pedigree = array()) {

  // get the subject pedigree array with all children ( no shared children)
  $full_array =  germplasm_manage_generate_subject_pedigree_array_noshared ($current_stock_id, $total_levels);
  
  $child_locations = array();
  germplasm_manage_generate_subject_pedigree_array_add_shared($full_array, $child_locations, array());
  
  // set all original children which were shared: shared = TRUE
  if (!empty($child_locations['shared'])) {
	  foreach ($child_locations['shared'] as $stock_id) {
  	  germplasm_manage_set_pedigree_node_shared($child_locations[$stock_id], $full_array);
  	}
  }
  
  return $full_array;
}

/** 
 * Generates a subject pedigree array with repeated children and no shared_children array. This is 
 * the first step in generated the full subject pedigree array.
 *
 * @param $current_stock_id
 *   Stock relationships where the object_id is the current_stock_id
 * @param $total_levels
 *   The maximum depth to travel
 * @param $pedigree
 *   The pedigree array generated so far. This is used for recursive function calls
 * @param $curr_level
 *   The current level. This is used for recursive function calls.
 * @return
 *   A pedigree array where the initial current_stock_id is the deepest stock
 */
function germplasm_manage_generate_subject_pedigree_array_noshared ($current_stock_id, $total_levels, $pedigree = array(), $curr_level = 1) {
	global $base_path;

  // get the current stock
  $curr_stock = tripal_core_generate_chado_var('stock',array('stock_id'=>$current_stock_id));
  
  //get the relationships where the current stock is the object
  $curr_relationships = tripal_core_generate_chado_var('stock_relationship',array('object_id'=>$current_stock_id));
  if (is_object($curr_relationships)) {
    // there is only one relationship
    // need to be an array of relationships
    $curr_relationships = array($curr_relationships);
  } elseif (empty($curr_relationships)) {
    $curr_relationships = array();
  }
  usort($curr_relationships, 'sort_stock_relationships_by_type');
  
  
  // create the node for the current stock
  $curr_stock = array(
        'stock_id' => $curr_stock->stock_id,
        'nid' => $curr_stock->nid,
        'link' => $base_path.'node/' . $curr_stock->nid,
        'name' => $curr_stock->name,
  );
  
  if (!$pedigree) {
    $curr_stock['children'] = array();
  } else {
    $curr_stock['children'][] = $pedigree;
  }
  
  // needed so no error is even thrown for foreach
  if (empty($curr_relationships)) {
    // just return current stock
    return array($curr_stock);
  }
  
  // if we are currently in the last level
  if ($curr_level >= $total_levels) {
    // just return current stock
    return array($curr_stock);
    
  }
  
  // foreach relationship where the current stock is the object
  $nxt_relationships = array();
  $i = 0;
  $curr_level = $curr_level + 1;
  foreach ($curr_relationships as $r) {
    // create a node for the subject of the relationship
    $nxt_stock = array(
      'stock_id' => $r->subject_id->stock_id,
      'nid' => $r->subject_id->nid,
      'link' => $base_path.'node/' . $r->subject_id->nid,
      'name' => $r->subject_id->name,
    );
    $nxt_stock['children'] = array($curr_stock);
    $nxt_stock['children'][0]['relationship_type'] = $r->type_id->name;

    // if we are currently in the last level
    if ($curr_level >= $total_levels) {
      // just return nxt_stock
      $nxt_relationships[] = $nxt_stock;
      
    } else {
  
      // get all relationships where the subject of the current relationship is the object
      $nxt_rels = tripal_core_generate_chado_var('stock_relationship',array('object_id'=>$r->subject_id->stock_id));
      if (is_object($nxt_rels)) {
        // there is only one relationship
        // need to be an array of relationships
        $nxt_rels = array($nxt_rels);
      } elseif (empty($curr_relationships)) {
        $nxt_rels = array();
      }
      
      if ($nxt_rels) { 
        usort($nxt_rels, 'sort_stock_relationships_by_type');
        
        // get the pedigree for each of the subjects (RECURSUION)
        $curr_level = $curr_level + 1;
        foreach ($nxt_rels as $rr) {
          $nxt_stock['relationship_type'] = $rr->type_id->name;
          $nxt_pedigree = germplasm_manage_generate_subject_pedigree_array_noshared($rr->subject_id->stock_id, $total_levels, $nxt_stock, $curr_level);
          if ($nxt_pedigree) {
            $nxt_relationships = array_merge($nxt_relationships, $nxt_pedigree);
          }
        }
        
      } else {
        // if there are no relationships then return the node for the subject of the relationship
        $nxt_relationships[] = $nxt_stock;
      }
    }
    
    $i++;
    
  }

  return $nxt_relationships;
}

/**
 * Finds duplicate children and changes all but the first occurance to shared_children. This is the 
 * second step in producing the full subject pedigree array.
 *
 * @param $pedigree
 *   The pedigree generated by germplasm_manage_generate_subject_pedigree_array_noshared()
 * @param $child_locations
 *   An associative array where the keys are stock_ids and the values are location arrays. This 
 *   is added to recursively and should contain all children once. A location array contains all the
 *   keys needed to reach a child node. For example, the location array for 
 *   $pedigree[0]['children'][1] is array(0,'children',1).
 * @param $current_location
 *   The location array describing the current location in the tree. This function is used for
 *   recursive function calls.
 *   
 * NOTE: Rather then return the changed pedigree array, this function takes the reference to the
 * pedigree array as a parameter and changes it directly as it goes
 */
function germplasm_manage_generate_subject_pedigree_array_add_shared(&$pedigree, &$child_locations, $current_location) {

  // There are two states we can be in:
  // An array of stock nodes or in a single stock node
  // in order to change from a child to a shared_child we need to be in a stock node
  
  // If we are in an array of stock nodes
  if (isset($pedigree[0])) {
    //Recursively travel to each stock node
    foreach ($pedigree as $k => $stock_node) {
      germplasm_manage_generate_subject_pedigree_array_add_shared($pedigree[$k], $child_locations, array_merge($current_location, array($k)));
    }
  
  // Else we are in a single stock node
  } else {
    // Add the current stock to child locations if it's not already
    if (!isset($child_locations[$pedigree['stock_id']])) {
      $child_locations[$pedigree['stock_id']] = $current_location;
    }
    
    if (is_array($pedigree['children'])) {
      // foreach of the children, check if they should be shared
      foreach ($pedigree['children'] as $k => $child_node) {
        // if this is the first time this stock was encountered
        // add its location to the child locations array
        if (!isset($child_locations[$child_node['stock_id']])) {
          $child_locations[$child_node['stock_id']] = array_merge($current_location, array('children',$k));
        
          //Recursively go to this node
          germplasm_manage_generate_subject_pedigree_array_add_shared($pedigree['children'][$k], $child_locations, array_merge($current_location, array('children', $k)));
        
        // this stock has been encountered before
        // thus change this to a shared child
        } else {
          $pedigree['shared_children'][] = array(
            'location' => $child_locations[$child_node['stock_id']],
            'relationship_type' => $child_node['relationship_type'],
          );
          unset($pedigree['children'][$k]);

          // set original child in queueu to be shared
          $child_locations['shared'][] = $child_node['stock_id'];
        }
      }
    }
  }
  
  return $pedigree;

}

/**
 * Sets the 'shared' property of the node indicated by the location array to TRUE
 *
 * @param $locations
 *   A location array which contains all the keys needed to reach a child node. For example, the 
 *   location array for $pedigree[0]['children'][1] is array(0,'children',1)
 * @param $pedigree
 *   The full pedigree including shared_children as returned by 
 *   germplasm_manage_generate_subject_pedigree_array_add_shared()
 *
 * NOTE: Rather then return the changed pedigree array, this function takes the reference to the
 * pedigree array as a parameter and changes it directly as it goes
 */
function germplasm_manage_set_pedigree_node_shared($locations, &$pedigree) {
  
  $node = &$pedigree;
  foreach ($locations as $key) {
    $node = &$node[$key];
  }
  $node['shared'] = TRUE;
  
}

////////////////////////////////////////////////////////////////////////////////
// Custom Sort Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * A comparison function which sorts stock relationships based on type
 *
 * Order (relationship type): 
 *   1. is_maternal_parent_of
 *   2. is_paternal_parent_of
 *   3. all other relationship types
 *
 * @param $a
 *   One element of the array generated by tripal_core_generate_chado_var 
 *   where table=stock_relationship
 * @param $b
 *   Another element of the array generated by tripal_core_generate_chado_var 
 *   where table=stock_relationship
 * @return
 *   if $a comes before $b, -1
 *   if $a = $b, 0
 *   if $a comes after $b, 1
 */
function sort_stock_relationships_by_type ($a,$b) {

  // if either element is maternal_parent then it should go first
  if (preg_match('/maternal/',$a->type_id->name)) {
    return -1;
  } elseif (preg_match('/maternal/',$b->type_id->name)) {
    return 1;
  } else {
  
    // if neither element is maternal_parent
    // then paternal_parent should go first
    if (preg_match('/paternal/',$a->type_id->name)) {
      return -1;
    } elseif (preg_match('/paternal/',$b->type_id->name)) {
      return 1;
    } else {
      return 0;
    }
  
  }
}

////////////////////////////////////////////////////////////////////////////////
// Generate the textual representation for a pedigree array
////////////////////////////////////////////////////////////////////////////////

/**
 * Generates a textual representation of the pedigree array passed in
 *
 * For example, this function would return Serena Silverio // Ted Whitner / Matt Smith 
 * if the following relationships existed: 
 *  - Serena Silverio x Terd Whitner => Hillary Haddox
 *  - Hillary Haddox x Matt Smith => Margery Schimer
 * Which would produce a pedigree array of:
 * @code
    $pedigree = array(
      0 => array(
        'name' => 'Serena Silverio',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762272',
        'children' => array(
          0 => array(
            'name' => 'Hillary Haddox',
            'link' => 'http://knowpulse2.usask.ca/devel/node/1762276',
            'relationship_type' => 'is_maternal_parent_of',
            'children' => array(
              0 => array(
                'name' => 'Margery Schirmer',
                'link' => 'http://knowpulse2.usask.ca/devel/node/1762275',
                'relationship_type' => 'is_maternal_parent_of',
                'children' => array(),
                'shared' => 'TRUE'
              ),            
            ),
            'shared' => 'TRUE',
          ),
        ),
      ),
      1 => array(
        'name' => 'Ted Whitner',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762274',
        'children' => array(),
        'shared_children' => array(
          0 => array(
            'location' => array(0,'children',0),
            'relationship_type' => 'is_paternal_parent_of'
          )
        )
      ),      
      2 => array(
        'name' => 'Matt Smith',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762273',
        'children' => array(),
        'shared_children' => array(
          0 => array(
            'location' => array(0,'children',0,'children',0),
            'relationship_type' => 'is_paternal_parent_of'
          )            
        ),
      ),
    ); 
 * @endcode
 *
 * @param $pedigree
 *   A pedigree array as generated by germplasm_manage_generate_pedigree_array()
 * @param $textual_string_array
 *   [OPTIONAL] An array as generated by germplasm_manage_pedigree_get_textual_string_array($pedigree)
 * @param $cur_name
 *   The name of the current stock; this is used in recursive calls
 * @param $level
 *   The current level of the pedigee; this is used in recursive calls
 *
 * @return
 *   A textual representation of the pedigree array passed in
 */
function germplasm_manage_generate_textual_pedigree_string($pedigree, $total_levels, $textual_string_array = NULL, $cur_name = NULL, $level = NULL) {
  
  if (empty($textual_string_array)) {
    $textual_string_array = germplasm_manage_pedigree_get_textual_string_array($pedigree);
    $cur_name = $textual_string_array['leaf'];
    $level = $total_levels - 1;
  }
  
  if (!empty($textual_string_array['maternal_parent'][$cur_name])) {
    $maternal = germplasm_manage_generate_textual_pedigree_string($pedigree, $total_levels, $textual_string_array, $textual_string_array['maternal_parent'][$cur_name], $level - 1);
  } else {
    return $cur_name;
  }
  
  if (!empty($textual_string_array['paternal_parent'][$cur_name])) {
    $paternal = germplasm_manage_generate_textual_pedigree_string($pedigree, $total_levels, $textual_string_array, $textual_string_array['paternal_parent'][$cur_name], $level - 1);
  } else {
    return $cur_name;
  }
  
  $string = $maternal . ' ' . str_repeat('/',$level) . ' ' . $paternal;
  
  return $string;  
  
}

/**
 * Generate an array describing the pedigree relationships in a more compact form
 *
 * @param $pedigree
 *   An array generated by germplasm_manage_generate_pedigree_array() describing the pedigree
 * @param $full_pedigree
 *   [OPTIONAL] the full pedigree; this is used throughout recursion 
 *   and is the same as $pedigree on the first call to this function
 *
 * @return
 *   An array describing the pedigree relationships in a more compact form
 *
 * Example Usage:
 * @code
    $pedigree = array(
      0 => array(
        'name' => 'Serena Silverio',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762272',
        'children' => array(
          0 => array(
            'name' => 'Hillary Haddox',
            'link' => 'http://knowpulse2.usask.ca/devel/node/1762276',
            'relationship_type' => 'is_maternal_parent_of',
            'children' => array(
              0 => array(
                'name' => 'Margery Schirmer',
                'link' => 'http://knowpulse2.usask.ca/devel/node/1762275',
                'relationship_type' => 'is_maternal_parent_of',
                'children' => array(),
                'shared' => 'TRUE'
              ),            
            ),
            'shared' => 'TRUE',
          ),
        ),
      ),
      1 => array(
        'name' => 'Ted Whitner',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762274',
        'children' => array(),
        'shared_children' => array(
          0 => array(
            'location' => array(0,'children',0),
            'relationship_type' => 'is_paternal_parent_of'
          )
        )
      ),      
      2 => array(
        'name' => 'Matt Smith',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762273',
        'children' => array(),
        'shared_children' => array(
          0 => array(
            'location' => array(0,'children',0,'children',0),
            'relationship_type' => 'is_paternal_parent_of'
          )            
        ),
      ),
    ); 
    $textual_string_array = germplasm_manage_pedigree_get_textual_string_array($pedigree);

    //dump of $textual_string_array
    textual string array => Array
    (
        [maternal_parent] => Array
            (
                [Hillary Haddox] => Serena Silverio
                [Margery Schirmer] => Hillary Haddox
            )
    
        [leaf] => Margery Schirmer
        [paternal_parent] => Array
            (
                [Hillary Haddox] => Ted Whitner
                [Margery Schirmer] => Matt Smith
            )
    
    )    
 * @endcode
 */
function germplasm_manage_pedigree_get_textual_string_array($pedigree, $full_pedigree = NULL) {
	
	if (empty($full_pedigree)) {
	  $full_pedigree = $pedigree;
	  $base = TRUE;
	}
	
	//Catagorize
	$catagory = '';
	if (isset($pedigree['children'])) {
	  if (empty($pedigree['children'])) {
	    $catagory = 'leaf';
	  } else {
	    $catagory = 'with_children';
	  }
	} else {
	  $catagory = 'array_of_stocks';
	}
	if (isset($pedigree['shared_children'])) {
	  if (!empty($pedigree['shared_children'])) {
	    $catagory = 'with_children';
	  }
	}	
	
	// Base Case: Reached THE Leaf (ie: current node is a stock node with no children)
  switch($catagory) {
    case 'leaf':
		  return array('leaf' => $pedigree['name']);
	  break;
	// There are two non-leaf type nodes we could be looking at
	//    1) stock node with children
    case 'with_children':
    	foreach($pedigree['children'] as $child_node) {
    	  if ($child_node['relationship_type'] == 'is_maternal_parent_of') {
      	  $curr_result['maternal_parent'][$child_node['name']] = $pedigree['name'];
      	} elseif ($child_node['relationship_type'] == 'is_paternal_parent_of') {
      	  $curr_result['paternal_parent'][$child_node['name']] = $pedigree['name'];
      	}
    	}
    	if (isset($pedigree['shared_children'])) {
        foreach($pedigree['shared_children'] as $child_node) {
          if ($child_node['relationship_type'] == 'is_maternal_parent_of') {
            $curr_result['maternal_parent'][germplasm_manange_pedigree_get_name($child_node['location'], $full_pedigree)] = $pedigree['name'];
          } elseif ($child_node['relationship_type'] == 'is_paternal_parent_of') {
            $curr_result['paternal_parent'][germplasm_manange_pedigree_get_name($child_node['location'], $full_pedigree)] = $pedigree['name'];
          }
        }    	
    	}
    	$result = germplasm_manage_pedigree_get_textual_string_array($pedigree['children'], $full_pedigree);
      if (!empty($result['maternal_parent'])) {
        foreach ($result['maternal_parent'] as $child => $parent) {
          $curr_result['maternal_parent'][$child] = $parent;
        }
      } 
      if (!empty($result['paternal_parent'])) {
        foreach ($result['paternal_parent'] as $child => $parent) {
          $curr_result['paternal_parent'][$child] = $parent;
        }
      }
      if (!empty($result['leaf'])) {
        $curr_result['leaf'] = $result['leaf'];
      }
    break;
	//    2) array of stock nodes
    case 'array_of_stocks':
      $curr_result = array();
      foreach ($pedigree as $k => $stock_node) {
        $result = germplasm_manage_pedigree_get_textual_string_array($stock_node, $full_pedigree);
        if (!empty($result['maternal_parent'])) {
          foreach ($result['maternal_parent'] as $child => $parent) {
            $curr_result['maternal_parent'][$child] = $parent;
          }
        } 
        if (!empty($result['paternal_parent'])) {
          foreach ($result['paternal_parent'] as $child => $parent) {
            $curr_result['paternal_parent'][$child] = $parent;
          }
        }
        if (!empty($result['leaf'])) {
          $curr_result['leaf'] = $result['leaf'];
        }
      }
    break;
  } //end of case

  return $curr_result;
}

/**
 * Retrieve the name of a pedigree node described by a locations array
 *
 * @param $location
 *   An array of the keys of a given pedigree node
 * @param $full_pedigree
 *   An array generated by germplasm_manage_generate_pedigree_array() describing the pedigree
 *
 * @return 
 *   The name of the pedigree node described by a locations array
 *
 * Example Usage:
 * @code
    $pedigree = array(
      0 => array(
        'name' => 'Serena Silverio',
        'link' => 'http://knowpulse2.usask.ca/devel/node/1762272',
        'children' => array(
          0 => array(
            'name' => 'Hillary Haddox',
            'link' => 'http://knowpulse2.usask.ca/devel/node/1762276',
            'relationship_type' => 'is_maternal_parent_of',
            'children' => array(
              0 => array(
                'name' => 'Margery Schirmer',
                'link' => 'http://knowpulse2.usask.ca/devel/node/1762275',
                'relationship_type' => 'is_maternal_parent_of',
                'children' => array(),
                'shared' => 'TRUE'
              ),            
            ),
            'shared' => 'TRUE',
          ),
        ),
      ),
    );
    $locations = array(0, 'children',0,'children',0);
    $name1 = germplasm_manange_pedigree_get_name ($location, $pedigree);

    $locations = array(0, 'children',0);
    $name2 = germplasm_manange_pedigree_get_name ($location, $pedigree);
 * @endcode
 * The above code will return a $name1 of Margery Schirmer and a $name2 of Hillary Haddox
 */
function germplasm_manange_pedigree_get_name ($location, $full_pedigree) {
  
  $node = $full_pedigree;
  foreach($location as $key) {
    $node = $node[$key];
  }
  
  return $node['name'];
  
}


////////////////////////////////////////////////////////////////////////////////
// End of Software
////////////////////////////////////////////////////////////////////////////////




