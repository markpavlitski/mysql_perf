<?php

function mysql_perf_report() {
  $output = '';

  $output .= '<h2>' . t('Most frequent queries') . '</h2>';
  $output .= mysql_perf_most_frequent_queries();

  $output .= '<h2>' . t('Longest running queries') . '</h2>';
  $output .= mysql_perf_longest_queries();

  $output .= '<h2>' . t('Longest running queries (aggregated)') . '</h2>';
  $output .= mysql_perf_longest_queries_aggregated();

  $output .= '<h2>' . t('Most queries per page') . '</h2>';
  $output .= mysql_perf_queries_per_page();

  $output .= '<h2>' . t('Most frequent queries per request') . '</h2>';
  $output .= mysql_perf_most_frequent_queries_per_request();

  $output .= '<h2>' . t('Longest running queries per request') . '</h2>';
  $output .= mysql_perf_longest_running_queries_per_request();

  return $output;
}

function mysql_perf_most_frequent_queries() {
  $rows = array();
  $header = array(t('Query'), t('Count'), t('Total execution time'));
  $result = db_query("SELECT qid, query, COUNT(*), SUM(execution_time) FROM {mysql_perf_raw} GROUP BY query ORDER BY COUNT(*) DESC LIMIT 10");
  foreach ($result as $query) {
    $row = (array)$query;
    unset($row['qid']);
    $row['query'] = theme('mysql_perf_query', array('qid' => $query->qid, 'query' => $query->query));
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function mysql_perf_longest_queries_aggregated() {
  $rows = array();
  $header = array(t('Query'), t('Count'), t('Total execution time'));
  $result = db_query("SELECT qid, query, COUNT(*), SUM(execution_time) FROM {mysql_perf_raw} GROUP BY query ORDER BY SUM(execution_time) DESC LIMIT 10");
  foreach ($result as $query) {
    $row = (array)$query;
    unset($row['qid']);
    $row['query'] = theme('mysql_perf_query', array('qid' => $query->qid, 'query' => $query->query));
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function mysql_perf_longest_queries() {
  $rows = array();
  $header = array(t('Query'), t('Execution time'));
  $result = db_query("SELECT qid, query, MAX(execution_time)FROM {mysql_perf_raw} GROUP BY query ORDER BY MAX(execution_time) DESC LIMIT 10");
  foreach ($result as $query) {
    $row = (array)$query;
    unset($row['qid']);
    $row['query'] = theme('mysql_perf_query', array('qid' => $query->qid, 'query' => $query->query));
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function mysql_perf_queries_per_page() {
  $rows = array();
  $header = array(t('URL'), t('Total queries'), t('Total execution time'));
  $result = db_query("SELECT url, COUNT(*), SUM(execution_time) FROM {mysql_perf_raw} GROUP BY url ORDER BY COUNT(*) DESC LIMIT 10");
  foreach ($result as $query) {
    $rows[] = (array)$query;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function mysql_perf_most_frequent_queries_per_request() {
  $rows = array();
  $header = array(t('Url'), t('Query'), t('Count'), t('Total execution time'));
  $result = db_query("SELECT qid, url, query, COUNT(query), SUM(execution_time) FROM {mysql_perf_raw} GROUP BY query, uuid ORDER BY COUNT(query) DESC LIMIT 10");
  foreach ($result as $query) {
    $row = (array)$query;
    unset($row['qid']);
    $row['query'] = theme('mysql_perf_query', array('qid' => $query->qid, 'query' => $query->query));
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function mysql_perf_longest_running_queries_per_request() {
  $rows = array();
  $header = array(t('Url'), t('Query'), t('Count'), t('Total execution time'));
  $result = db_query("SELECT qid, url, query, COUNT(query), SUM(execution_time) FROM {mysql_perf_raw} GROUP BY query, uuid ORDER BY COUNT(query) DESC LIMIT 10");
  foreach ($result as $query) {
    $row = (array)$query;
    unset($row['qid']);
    $row['query'] = theme('mysql_perf_query', array('qid' => $query->qid, 'query' => $query->query));
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function mysql_perf_query_report($query) {
  $output = '';

  $query_string = trim($query->query);
  $query_type = _mysql_perf_query_type($query);

  //$output .= check_plain($query_string);

  $output .= '<h2>' . t('Explain') . '</h2>';
  $header = array(t('Select type'), t('Table'), t('Type'), t('Possible keys'), t('Key'), t('Key len'), t('Ref'), t('Rows'), t('Extra'));
  if ($query_type == 'SELECT') {
    $explain = array();
    $queries = db_query("SELECT qid FROM {mysql_perf_raw} WHERE query = :query GROUP BY query_raw ORDER BY timestamp DESC LIMIT 20", array(':query' => $query->query));
    foreach ($queries as $q) {
      $q = mysql_perf_qid_load($q->qid);
      $q_string = _mysql_perf_query_substitute($q);
      $e = (array)db_query("EXPLAIN " . $q_string)->fetchObject();
      unset($e['id']);
      $explain[] = array(array('colspan' => count($header), 'data' => check_plain($q_string)));
      $explain[] = $e;
    }
    if (count($explain) >= 200) {
      $explain[] = array(array('colspan' => count($header), 'data' => '...'));
    }
  }
  else {
    $explain = array(array(array('colspan' => count($header), 'data' => t('EXPLAIN is not supported for %query_type queries.', array('%query_type' => $query_type)))));
  }
  $output .= theme('table', array('header' => $header, 'rows' => $explain));

  return $output;
}
