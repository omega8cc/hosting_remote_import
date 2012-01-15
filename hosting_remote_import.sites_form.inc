<?php

/**
 * Form to start the process of importing some sites
 */
function hosting_remote_import_sites_form(&$form_state, $node) {
  $form = array();
  
  // Horrible, horrible way that we have to redirect after a batch.
  if (!empty($_SESSION['hosting_remote_import_redirect'])) {
    $url = $_SESSION['hosting_remote_import_redirect'];
    unset($_SESSION['hosting_remote_import_redirect']);
    drupal_goto($url);
  }
  
  // @todo: Make sure this server has the remote_import service.
  
  $form['description'] = array(
    '#value' => t('Description goes here'),
  );
  
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Retrieve remote sites'),
  );
  
  
  return $form;
}

/**
 * Form submit callback.
 *
 * @see hosting_remote_import_sites_form()
 */
function hosting_remote_import_sites_form_submit($form, &$form_state) {
  // Set up a batch that will wait for the hosting task to complete.
  
  $batch = array(
    'operations' => array(
      array('hosting_remote_import_sites_form_submit_batch_wait_task', array($form_state['values']['nid'])),
    ),
    'finished' => 'hosting_remote_import_sites_form_submit_batch_finished',
    'file' => drupal_get_path('module', 'hosting_remote_import') . '/hosting_remote_import.sites_form.inc',
  );
  
  batch_set($batch);
}

function hosting_remote_import_sites_form_submit_batch_wait_task($nid, &$context) {
  
  if (!isset($context['results']['vid'])) {
    $server = node_load($nid);
  
    // Create a new list remote sites task.
    $task = hosting_add_task($nid, 'remote_import_list_sites');
  
    // Save this task into the context, so we can keep track of it.
    $context['results']['vid'] = $task->vid;
    $context['results']['nid'] = $nid;
    $context['finished'] = 0.1;
    return;
  }
  
  
  $tasks = db_query("SELECT t.vid, n.nid FROM {hosting_task} t INNER JOIN {node} n ON n.vid = t.vid WHERE task_type = '%s' AND t.rid = %d AND t.vid >= %d ORDER BY t.vid ASC", 'remote_import_list_sites', $nid, $context['results']['vid']);
  while ($row = db_fetch_object($tasks)) {
    $task = node_load($row->nid, $row->vid);
    
    switch ($task->task_status) {
      case HOSTING_TASK_PROCESSING:
        $context['finished'] = 0.8;
        $context['message'] = 'Fetching results from remote server';
        break;
      case HOSTING_TASK_QUEUED:
        $context['finished'] = 0.2;
        $context['message'] = 'Task is queued, waiting for dispatcher to run.';
        break;
      case HOSTING_TASK_SUCCESS:
        $context['message'] = 'Complete';
        $context['results']['final_vid'] = $task->vid;
        break;
      case HOSTING_TASK_ERROR:
      default:
        return;
    }
    
  }
}

function hosting_remote_import_sites_form_submit_batch_finished($success, $results, $operations) {
  if (isset($results['final_vid'])) {
    $vid = $results['final_vid'];
    $nid = $results['nid'];
    // This is horrible, but this is the only way that Batch API lets me do
    // this.
    $_SESSION['hosting_remote_import_redirect'] = 'node/' . $nid . '/import-sites/chooser/' . $vid . '/' . drupal_get_token($vid);
  }
  else {
    drupal_set_message(t('An error occurred, review the task log and try-again.'), 'error');
  }
  
}
