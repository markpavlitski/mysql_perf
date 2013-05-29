<?php

function theme_mysql_perf_query($variables) {
  $qid = intval($variables['qid']);
  return l($variables['query'], 'admin/reports/mysql_perf/' . $qid, $variables['options']);
}
