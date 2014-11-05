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

    // Define Migration source.
    $path = drupal_get_path('module', 'tr_migrate') . '/import';
    $this->source = new MigrateSourceCSV("$path/basic_page.csv", array(), array('header_rows' => 1));

    // Deal with issues with csv line endings.
    ini_set('auto_detect_line_endings', TRUE);

    // Define Migration Destination.
    $this->destination = new MigrateDestinationNode('page');

    // Define Migration mapping.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationNode::getKeySchema());

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('body', 'body');
  }
}
