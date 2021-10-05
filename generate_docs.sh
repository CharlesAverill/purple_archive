git checkout gh-pages
git merge main
doxygen .doxygen_cfg
git add *
git commit -m "$1"
git push origin gh-pages
git checkout main
