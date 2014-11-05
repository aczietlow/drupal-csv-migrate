<?php
/**
 * @file
 * TR demo migration.
 *
 * @copyright (C) Copyright 2014 Palantir.net, Inc.
 */

class TrDemoMigration extends Migration {
  /**
   * Constructor for TrDemoMigration.
   */
  public function __construct() {
    parent::__construct();
    $path = drupal_get_path('module', 'tr_migration') . '/import';

    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationNode('provision');
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationNode::getKeySchema(),
      'kroc_migration'
    );

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('field_body', 'provision_text');
    $this->addFieldMapping('field_provision_type', 'provision_type');
  }

  /**
   * Prepares the row data before field mapping.
   *
   * @param Object $row
   *   Object containing raw source data.
   *
   * @return bool
   *   TRUE to process this row, FALSE to have the source skip it.
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // Provision title will use the pattern '<provision type>: <accord name>'.
    $row->title = $row->provision_type . ': ' . $row->accord_name;

    return TRUE;
  }

  /**
   * Preforms actions after the migration entity is saved to Drupal.
   *
   * After the provision entity is saved in the Drupal database
   * we can build the relationship from the peace accord to the new
   * provision. No additional functionality is needed to on rollback
   * because when the provision entity is deleted the entity reference
   * will no longer exist. Entity reference fields don't maintain the
   * entity reference after an entity has been deleted.
   *
   * @param stdClass $entity
   *   Newly saved Drupal entity.
   *
   * @param stdClass $row
   *   Row object from the database query.
   */
  public function complete($entity, stdClass $row) {
    $query = new EntityFieldQuery();

    $query->entityCondition('entity_type', 'node');
    $query->propertyCondition('type', 'peace_accord');
    $query->propertyCondition('title', $row->accord_name);
    $result = $query->execute();

    $accord_id = array_keys($result['node']);
    $accord_node = node_load(array_shift(array_values($accord_id)));

    $entity_wrapper = entity_metadata_wrapper('node', $accord_node);

    $referenced_provisions = $entity_wrapper->field_entity_reference_provision->value();

    $first_ref_provision = array_shift(array_values($referenced_provisions));

    // If first item in array is null, reset array.
    if ($first_ref_provision == NULL) {
      $referenced_provisions = array();
    }

    $referenced_provisions = empty($referenced_provisions) ? array() : $referenced_provisions;
    $referenced_provisions[] = node_load($entity->nid);

    $entity_wrapper->field_entity_reference_provision->set($referenced_provisions);
    $entity_wrapper->save();
  }
}