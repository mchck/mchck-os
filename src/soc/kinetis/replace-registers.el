(defun 2c-replace-registers ()
  (interactive)
  (re-search-forward "\\_<\\([[:upper:]]+\\)\\([[:digit:]]?\\)[.]\\([[:alnum:]_]*\\)[.]\\([[:alnum:]_]*\\)\s *=\s *\\([[:digit:]]+\\);")
  (let* ((periph (match-string 1))
         (periph-num (match-string 2))
         (reg (upcase (match-string 3)))
         (field (upcase (match-string 4)))
         (val (match-string 5)))
    (if (equal field "RAW")
        (replace-match (format "%s%s_%s = %s;"
                               periph periph-num reg
                               val))
      (replace-match (format "bf_set(%s%s_%s, %s_%s_%s, %s);"
                             periph periph-num reg
                             periph reg field
                             val)))))

(defun 2c-extract-enums ()
  (interactive)
  (re-search-forward "\\(enum \\)\\([[:alnum:]_]+ \\)?\\({[^}]+}\\)")
  (save-excursion
    (goto-char (point-max))
    (let ((start (point)))
      (insert "\n")
      (insert (concat (match-string 1) (match-string 3) ";\n"))
      (indent-region start (point)))))
