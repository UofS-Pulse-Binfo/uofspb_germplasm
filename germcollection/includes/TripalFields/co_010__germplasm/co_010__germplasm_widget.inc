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
    // Add scripts and style.
    $field_theme_dir = drupal_get_path('module', 'germcollection') . '/includes/TripalFields/co_010__germplasm/theme/'; 
    // Library, style and scripts.
    drupal_add_library('system', 'drupal.ajax');
    drupal_add_library('system', 'ui.autocomplete');
        
    $widget['#attached']['css'] = array($field_theme_dir . 'style_stockcollection.css');
    $widget['#attached']['js'] = array($field_theme_dir . 'script_stockcollection.js');


    // # FIELDSET: main fieldset.
    $widget['main_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Germplasm'),
      '#id' => 'stock-collection-field-container',
    );

      // Warn user (admin), to review germplasm collection list before
      // clicking submit button as modifying list after would require elevated permission.
      // Will be done manually through update query.
      $widget['main_fieldset']['reminder'] = array(
        '#markup' => '<div class="messages warning">Please review the final Germplasm List before clicking Save button.
          To add or remove germplasm from a Stock Collection will require permission.</div>'
      );

      // # FIELD:  Select spieces.
      $species = germcollection_get_scientific_name();
      $widget['main_fieldset']['fld_select_species'] = array(
        '#type' => 'select',
        '#options' => array(0 => '- Scientific Name -') + $species,
        '#prefix' => '<div class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-select-species'
      );

      // # FIELD: Auto-complete search germplasm.
      // Disable everything when there is no species.
      $organism = 0;
      
      $disabled = (count($species) < 1) ? TRUE : FALSE; 
      $widget['main_fieldset']['fld_textfield_germplasm'] = array(
        '#type' => 'textfield',
        '#size' => 40,
        '#disabled' => $disabled,
        '#attributes' => array('placeholder' => t('Germplasm / Stock Name')),
        '#prefix' => '<div id="stock-collection-wrapper-fldgermplasm" class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-textfield-germplasm',
        //'#autocomplete_path' => 'ajax/tripal/autocomplete-stockname/' . $organism
      );

      // # FIELD: Add germplasm to collection (manual insert).
      // Use id to manage items in the list/table below.
      $container_id = 'stock-collection-germplasm-container';
      
      // This checkbox field acts as a clickable element with ajax functionality.
      // Type=button, link or submit breaks: callback class is incomplete.
      $widget['main_fieldset']['fld_button_add'] = array(
        '#type' => 'button',
        '#value' => t('Add to Collection'),
        '#disabled' => $disabled,
        '#prefix' => '<div id="wrapper-fld-button-add" class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#attributes' => array('onclick' => 'return (false);'),
        '#executes_submit_callback' => false,
        '#id' => 'stock-collection-fld-button-add',
      );

      // Save germplasm entered in this field.
      $widget['stock_collection_set_id'] = array(
        '#type' => 'hidden',
        '#default_value' => 0,
        '#attributes' => array('id' => 'stock-collection-set-id'),
      );

      // # LINK: Link to upload gerplasm in bulk.
      $link = l(t('Upload File'), '#', array('#attributes'));
      $widget['main_fieldset']['link_upload'] = array(
        '#markup' => '<div id="stock-collection-bulk-upload" class="stock-collection-fld-right">Bulk Germplasm: ' . $link . '</div>
          <div style="clear: both"></div>'
      );

      // # CONTAINER: Contain controls for bulk germplasm upload.
      $widget['main_fieldset']['container_bulk_upload'] = array(
        '#prefix' => '<div id="stock-collection-container-bulk-upload">', 
        '#suffix' => '</div>',
      );
      
        // # FIELD: Upload field object.
        $widget['main_fieldset']['container_bulk_upload']['fld_upload'] = array(
          '#type' => 'file',
          '#title' => t('FILE UPLOAD: Upload Bulk Germplasm File is a TSV format with UNIQUENAME, NAME and SCIENTIFIC NAME column headers.'),
          '#disabled' => $disabled,
        );
      
      
      // Container, list of germplasm active in the collection.
      $stock_collection = theme('table', array('sticky' => FALSE, 'header' => [], 'rows' => []));  

      // Summary header.
      $widget['main_fieldset']['header'] = array(
        '#markup' => '<div id="stock-collection-header">&#9205; Germplasm Collection: <span>0 Germplasm</span></div>'
      );

      // @see $id set in add collection button.
      $widget['main_fieldset']['container'] = array(
        '#markup' => '<div  id="' . $container_id . '">' . $stock_collection . '</div>',
      );  
    
    // Path:
    $path = array(
      'autocomplete' => '/ajax/tripal/autocomplete-stockname/',
      'stockprop' => '/ajax/tripal/stockprop/',
      'biodata' => '/bio_data/'
    );

    drupal_add_js(
      array('germcollection' => array('path' => $path)), 
      array('type' => 'setting')
    );

    return $widget;
  }
  /**
   * Ajax callback method - add stock name to germplasm collection.
   */
  public function stock_collection_add_ajax_callback($form, $form_state) {
    // Add stock name to this germplasm collection.
    return $form['main_fieldset']['container'];
  }

  /**
   * Ajax callback method - set species/organism to restrict autocomplete search.
   */  
  public function stock_collection_select_ajax_callback($form, $form_state) {
    // Update autocomplete to attach selected species to narrow down
    // suggestion to only that match the organism.
    return $form['main_fieldset']['fld_textfield_germplasm'];
  }
  


  /**
   * @see ChadoFieldWidget::validate()
   *
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
  }

  /**
   * @see ChadoFieldWidget::submit()
   *
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
  }

}




