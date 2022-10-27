<?php
/**
 * @class
 * Purpose: Supply a germplasm list for a stockcollection.
 *
 * Allowing edit? No
 * Data:
 *   Details for each germplasm are stored in stock and stockprop. The
 *   connection between stockcollection and stocks is in stockcollection_stock.
 * Assumptions: None known.
 */


class co_010__germplasm_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Germplasm List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['co_010__germplasm'];


  /**
   * @see ChadoFieldWidget::form()
   *
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {     
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
  
    // # FIELDSET: main fieldset.
    // All form elements are contained in this fieldset.
    $widget['main_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Germplasm'),
      '#id' => 'stock-collection-field-container',
    );
      
      // Warn user (admin), to review germplasm collection list before
      // clicking submit button as modifying list after would require 
      // elevated permission and will be done manually through update query.
      $widget['main_fieldset']['reminder'] = array(
        '#markup' => '<div class="messages warning">' .
          t('Please review the final Germplasm List before clicking Save button. 
             To add or remove germplasm from a Stock Collection will require permission.')
        . '</div>'
      );

      // # FIELD:  Select spieces/organism.
      // Fetch all genus + species in organism table. 
      // @see api/germcollection.api.inc.
      $species = germcollection_get_scientific_name();
      $widget['main_fieldset']['fld_select_species'] = array(
        '#type' => 'select',
        '#title' => t('Scientific Name'),
        '#title_display' => 'invisible',
        '#options' => array(0 => '- Scientific Name -') + $species,
        '#prefix' => '<div class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-select-species'
      );

      // # FIELD: Auto-complete search germplasm.
      // Disable everything when there is no species.
      $disabled = (count($species) < 1) ? TRUE : FALSE; 

      $widget['main_fieldset']['fld_textfield_germplasm'] = array(
        '#type' => 'textfield',
        '#title' => t('Germplasm Name'),
        '#title_display' => 'invisible',
        '#size' => 30,
        '#maxlength' => 50,
        '#disabled' => $disabled,
        '#attached' => array(
          'library' => array(array('system', 'ui.autocomplete'))
        ),
        '#attributes' => array('placeholder' => t('Germplasm / Stock Name')),
        '#prefix' => '<div id="stock-collection-wrapper-fldgermplasm" class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-textfield-germplasm',
      );

      // # FIELD: Add germplasm to collection.
      // Use id to manage items in the list/table below.
      $container_id = 'stock-collection-germplasm-container';
      
      $widget['main_fieldset']['fld_button_add'] = array(
        '#type' => 'button',
        '#value' => t('Add to Collection'),
        '#disabled' => $disabled,
        '#attributes' => array('onclick' => 'return (false);'),
        '#executes_submit_callback' => false,
        '#prefix' => '<div class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-button-add',
      );
      
      // # FIELD: Collection field.
      // Save germplasm entered in this field.
      $widget['stock_collection_set_id'] = array(
        '#type' => 'hidden',
        '#default_value' => 0,
        '#attributes' => array('id' => 'stock-collection-set-id'),
      );

      // # LINK: Link to upload gerplasm in bulk.
      $link = l(t('Upload File'), '#', array('#attributes'));
      $widget['main_fieldset']['link_upload'] = array(
        '#markup' => '<div id="stock-collection-bulk-upload" class="stock-collection-fld-right">
          Bulk Germplasm: ' . $link . '</div><div style="clear: both"></div>'
      );

      // # CONTAINER: Contain controls for bulk germplasm upload.
      $widget['main_fieldset']['container_bulk_upload'] = array(
        '#prefix' => '<div id="stock-collection-container-bulk-upload">', 
        '#suffix' => '</div>',
      );
      
        // # FIELD: Upload field object.
        $widget['main_fieldset']['container_bulk_upload']['fld_upload'] = array(
          '#type' => 'file',
          '#title' => t('FILE UPLOAD: Upload Bulk Germplasm File is a 
            TSV format with UNIQUENAME, NAME and SCIENTIFIC NAME column headers.'),
          '#disabled' => $disabled,
        );
      
      
      // Container, list of germplasm active in the collection.
      $stock_collection = theme('table', array('sticky' => FALSE, 'header' => [], 'rows' => []));  

      // Summary header.
      $widget['main_fieldset']['header'] = array(
        '#markup' => '<div id="stock-collection-header">&#9205; Germplasm Collection: 
          <span>0</span> Germplasm <div id="stock-collection-wait"></div></div>'
      );

      // @see $id set in add collection button.
      $widget['main_fieldset']['container'] = array(
        '#markup' => '<div  id="' . $container_id . '">' . $stock_collection . '</div>',
      );  


    // Add scripts and style.
    $field_theme_dir = drupal_get_path('module', 'germcollection') 
      . '/includes/TripalFields/co_010__germplasm/theme/'; 
    
    $widget['#attached'] = array(
      'css' => array($field_theme_dir . 'style_stockcollection.css'),
      'js'  => array($field_theme_dir . 'script_stockcollection.js')
    );
    
    // Path as Drupal settings.
    $widget['germcollection']['#attached']['js'][] = array(
      'data' => array('germcollectionPath' => array(
        'autocomplete' => '/ajax/tripal/autocomplete-stockname/',
        'stockprop' => '/ajax/tripal/stockprop/',
        'biodata' => '/bio_data/'
      )),
      'type' => 'setting'
    );
  }

  /**
   * @see ChadoFieldWidget::validate()
   *
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $stock_collection = $form_state['values'][ $field_name ]['und'][ $delta ]['stock_collection_set_id'];
    if (empty($stock_collection)) {
      form_set_error('fld_select_species', 'Germplasm collection has 0 germplasm.');
    }
  }

  /**
   * @see ChadoFieldWidget::submit()
   *
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    parent::submit($form, $form_state, $entity_type, $entity, $langcode, $delta);
    $field_name = $this->field['field_name'];
    $stock_collection = $form_state['values'][ $field_name ]['und'][ $delta ]['stock_collection_set_id'];
    
    // Cache germplasm: at this point the stockcollection has not been 
    // created yet, storing the germplasm for callback function below.
    $entity->collection = $stock_collection;
  }
}


//////


/**
 * Entity insert callback. Insert all germplasm selected
 * from the interface into the stockcollection.
 */
function germcollection_entity_insert($entity, $type) {
  if ($type == 'TripalEntity' && !empty($entity->collection)) {
    $table = $entity->chado_table;
    $pk = $table . '_id';
    // Get lastest generated id in stockcollection table.
    $sql = sprintf("SELECT %s FROM {%s} ORDER BY %s DESC LIMIT 1", $pk, $table, $pk);
    $stockcollection_id = chado_query($sql)->fetchField();
    
    $TRANSACTION = db_transaction();

    try {
      $germplasm = explode(':', $entity->collection);  
      foreach($germplasm as $stock_id) {
        chado_insert_record($table . '_stock',
          array(
            'stockcollection_id' => $stockcollection_id,
            'stock_id' => $stock_id
          )
        );
      }
    }
    catch(Exception $e) {
      $TRANSACTION->rollback();
    }
  }
}




