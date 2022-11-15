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
      '#id' => 'germcollection-collection-create-container',
    );
      
      // Warn user (admin), to review germplasm collection list before
      // clicking submit button as modifying list after would require 
      // elevated permission and will be done manually through update query.
      $widget['main_fieldset']['reminder'] = array(
        '#markup' => '<div class="messages warning">' .
          t('Please review the final Germplasm List before clicking Save button. 
             To add or remove germplasm from a Stock Collection after it has been created
             would require permission.')
        . '</div>'
      );

      // # FIELD:  Select spieces/organism.
      // Fetch all genus + species in organism table. @see api/germcollection.api.inc.
      // A selection would return the stock id and stockname in the
      // form stock_id number:stock name (ie 123:Germ123).
      $species = germcollection_get_scientific_name();
      // Disable everything when there is no species or this form is in edit mode.
      $disabled = (count($species) < 1) ? TRUE : FALSE;
      $disabled = ($disabled === FALSE && $form['#entity']->chado_record_id > 0) ? TRUE : $disabled;

      $widget['main_fieldset']['fld_select_species'] = array(
        '#type' => 'select',
        '#disabled' => $disabled,
        '#options' => array(0 => '- Scientific Name -') + $species,
        '#id' => 'germcollection-fld-select-species',
      );

      // # FIELD: Autocomplete search germplasm.
      $widget['main_fieldset']['fld_textfield_germplasm'] = array(
        '#type' => 'textfield',
        '#maxlength' => 50,
        '#disabled' => $disabled,
        '#attributes' => array(
          'placeholder' => t('Germplasm / Stock Name')
        ),
        '#id' => 'germcollection-fld-textfield-germplasm',
      );

      // # FIELD: Button to add germplasm to collection.      
      $widget['main_fieldset']['fld_button_add'] = array(
        '#type' => 'button',
        '#value' => t('Add to Collection'),
        '#disabled' => $disabled,
        '#attributes' => array(
          'onclick' => 'return (false);'
        ),
        '#executes_submit_callback' => false,
        '#id' => 'germcollection-fld-button-add',
      );

      // # LINK: Link to upload gerplasm in bulk.
      $link = ($form['#entity']->chado_record_id > 0) ? '' : 'Bulk Germplasm: ' . l(t('Upload File'), '#');
      $widget['main_fieldset']['link_upload'] = array(
        '#type' => 'markup',
        '#markup' => '<span>' . $link . '</span>'
      );

      // # FIELD: Collection field.
      // Save germplasm/stock id entered into this field.
      $widget['germcollection_set_id'] = array(
        '#type' => 'hidden',
        '#default_value' => 0,
        '#attributes' => array(
          'id' => 'germcollection-set-id'
        ),
      );
      
      ////////

      // # CONTAINER: Collection table.
      // Container, list of germplasm active in the collection.
      
      // List all germplasm for viewing on edit mode.
      $rows = array();
      $collection_count = 0;

      if ($form['#entity']->chado_record_id > 0) {
        $collection_id = $form['#entity']->chado_record_id;
        $collection = germcollection_get_germplasm($collection_id);
        $collection_count = count($collection);

        foreach($collection as $stock) {
          $stock_name = ($stock[2]) ? l($stock[0], '/bio_data/' . $stock[2]) : $stock[0];

          $rows[] = array(
            $stock_name . ' ' . '<small>' . $stock[3] . ' ' . $stock[4] . '</small>',
          );
        }
      }

      $stock_collection = theme('table', array('sticky' => FALSE, 'header' => [], 'rows' => $rows));  

      // Collection summary table header.
      $widget['main_fieldset']['collecion_header'] = array(
        '#markup' => '<div id="germcollection-collection-header">
          &#9205; Germplasm Collection: <span>' . $collection_count . '</span> Germplasm 
          <div></div>
        </div>'
      );

      // Collection summary table.
      $widget['main_fieldset']['collection_table'] = array(
        '#markup' => '<div  id="germcollection-collection-table">' . $stock_collection . '</div>'
      ); 

      // Collection bulk germplasm upload.
      // # FIELD: Upload field object.
      $widget['main_fieldset']['collection_bulkfile_container'] = array(
        '#prefix' => '<div id="germcollection-bulk-upload">', 
        '#suffix' => '</div>',
      );

      $note_file = t('TSV file format (.tsv) with UNIQUENAME, NAME and SCIENTIFIC NAME 
                   (genus + species, ie. Lens culinaris) headers.');

      $widget['main_fieldset']['collection_bulkfile_container']['germcollection_file_id'] = array(
        '#type' => 'managed_file',
        '#title' => t('FILE UPLOAD: Upload Bulk Germplasm File'),
        '#disabled' => $disabled,
        '#upload_location' => 'public://germcollection/',
        '#upload_validators' => array(
          'file_validate_extensions' => array()
        ),
        '#description' => $note_file,
      );

    // # SETTINGS: 
    // Add scripts and style.
    $field_theme_dir = drupal_get_path('module', 'germcollection') . '/theme'; 
    
    $widget['#attached'] = array(
      'css' => array($field_theme_dir . '/style/style-collection-create.css'),
      'js'  => array($field_theme_dir . '/script/script-collection-create.js'),
      'library' => array(array('system', 'ui.autocomplete'))
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
    if (!isset($form_state['triggering_element']['#ajax']) && !isset($form['#entity']->chado_record_id)) {
      // Validate when save button has been clicked.
      $field_name = $this->field['field_name'];
      $stock_collection_set_id  = $form_state['values'][ $field_name ]['und'][ $delta ]['germcollection_set_id'];
      $stock_collection_file_id = $form_state['values'][ $field_name ]['und'][ $delta ]['main_fieldset']['collection_bulkfile_container']['germcollection_file_id'];
      
      if (empty($stock_collection_file_id) && empty($stock_collection_set_id)) {
        // No germplasm to save for this collection.
        form_set_error('fld_select_species', t('Germplasm collection has 0 germplasm.
          Please use the germplasm selector below.'));
      }
      
      if (!empty($stock_collection_file_id) && $stock_collection_file_id > 0) {
        // Bulk germplasm file provided, validate that entries entered do exist.
        $file = file_load($stock_collection_file_id);
        
        if ($file) {
          $file_path = drupal_realpath($file->uri);
          $file_extension = pathinfo($file_path)['extension'];
         
          // TSV:
          if (strtolower($file_extension) == 'tsv') {
            $handle = fopen($file_path, 'r');

            if ($handle) {
              $i = 0;
              while (!feof($handle)) {
                $cur_line = fgets($handle);

                if ($i == 0) { 
                  // Validate headers.
                  $headers = str_getcsv($cur_line, "\t");
                  if (strtolower($headers[0]) != 'uniquename' 
                      || strtolower($headers[1]) != 'name'
                      || strtolower($headers[2]) != 'scientific name') {
                    
                    $failed = t('Invalid column header found. Please remove file and upload again.');  
                    form_set_error('germcollection_bulk_file', $failed);
                  }
 
                  $i++;
                }
                else {
                  // 0:uniquename, 1:name, 2:scientific name (genus + sepecies).
                  $value = str_getcsv($cur_line, "\t");
                  $exits = germcollection_get_stock_id($value[0], $value[1], $value[2]);

                  if ($exits == 0) {
                    // Germplasm does not exists.
                    $failed = t('Germplasm collection file has germplasm that does not exist. LINE# ' . ($i + 1) . ' ' . $value[1] 
                      . ' Please remove file and upload again.');  
                    form_set_error('germcollection_bulk_file', $failed);
                  }
                
                  $i++;
                }
              }
              
              fclose($handle);
            }
          }
          else {
            // Incorrect file extension provided.
            $failed = t('Invalid germplasm collection file extension. Please remove file and upload again.');
            form_set_error('germcollection_bulk_file', $failed);
          }
        }
      }
    }
  }

  /**
   * @see ChadoFieldWidget::submit()
   *
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    parent::submit($form, $form_state, $entity_type, $entity, $langcode, $delta);
    
    if (!isset($form['#entity']->chado_record_id)) {
      $entity->collection = 0;
      $field_name = $this->field['field_name'];
      $stock_collection_set_id  = $form_state['values'][ $field_name ]['und'][ $delta ]['germcollection_set_id'];
      $stock_collection_file_id = $form_state['values'][ $field_name ]['und'][ $delta ]['main_fieldset']['collection_bulkfile_container']['germcollection_file_id'];
    
      // Bulk upload (file) has higher priority over manual upload.

      if ($stock_collection_set_id != 0) {
        // Cache germplasm: at this point the stockcollection has not been 
        // created yet, storing the germplasm for callback function below.
        $entity->collection = array('manual' => $stock_collection_set_id);
      }

      if (!empty($stock_collection_file_id) && $stock_collection_file_id > 0) {
        // Drupal managed file captures the file id - collection is the file id
        // of the bulf file provided.
        $entity->collection = array('file' => $stock_collection_file_id);
      }
    }
  }
}


////////


/**
 * Entity insert callback. Insert all germplasm selected
 * from the interface into the stockcollection.
 */
function germcollection_entity_insert($entity, $type) {
  if ($type == 'TripalEntity' && !empty($entity->collection) && !isset($entity->chado_record)) {
    if (is_array($entity->collection)) {
      $method = array_keys($entity->collection)[0];

      if (in_array($method, array('file', 'manual'))) {
        // Establish stock collection id inserted for use as
        // stockcollection_id (FK) in stockcollection_stock.
        $chado_table = $entity->chado_table;
        $pk = $chado_table . '_id';

        $sql = sprintf("SELECT %s FROM {%s} ORDER BY %s DESC LIMIT 1", $pk, $chado_table, $pk);
        $stockcollection_id = chado_query($sql)
          ->fetchField();
        
        if ($stockcollection_id > 0) { 
          $TRANSACTION = db_transaction();

          try {
            if ($method == 'file') {
              // Bulk file upload.
              $file = file_load($entity->collection['file']);
              $file_path = drupal_realpath($file->uri);
              
              $handle = fopen($file_path, 'r');
              $i = 0;
              while (!feof($handle)) {
                $cur_line = fgets($handle);

                if ($i == 0) {
                  // Skip column headers.
                  $i++;
                }
                else {
                  // 0:uniquename, 1:name, 2:scientific name (genus + sepecies).
                  $value = str_getcsv($cur_line, "\t");
                  $stock_id = germcollection_get_stock_id($value[0], $value[1], $value[2]);
                  
                  chado_insert_record($chado_table . '_stock',
                    array(
                      'stockcollection_id' => $stockcollection_id,
                      'stock_id' => $stock_id
                    )
                  );

                  $i++;
                }
              }
              // End bulk file upload.
            }
            else {
              // Manual entry. 
              $collection = explode(':', $entity->collection['manual']);  
              foreach($collection as $stock_id) {
                chado_insert_record($chado_table . '_stock',
                  array(
                    'stockcollection_id' => $stockcollection_id,
                    'stock_id' => $stock_id
                  )
                );
              }
              // End manual entry.
            }
          }
          catch(Exception $e) {
            $TRANSACTION->rollback();
            exit();
          }
        }
        else {
          // Stock collection was not created.
          drupal_set_message(t('Stock collection was not created.'), 'error');
          return 0;
        }
      }
      else {
        drupal_set_message(t('Unsupported method used to create a collection.'), 'error');
        return 0;
      }
    }      
  }
}