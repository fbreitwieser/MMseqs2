#!/bin/sh -e

notExists() {
	  [ ! -f "$1" ]
}

hasCommand() {
	  command -v "$1" >/dev/null 2>&1 || { echo "Please make sure that $1 is in \$PATH."; exit 1; }
}

hasCommand wget
hasCommand awk
hasCommand zcat
hasCommand touch
hasCommand tar

DLNAME="$1"
#TMP_PATH="$1.tmp"
IS_UNIPROT=0

FNAME="meh"
DBNAME="meh"
if [ "$DLNAME" -eq "uniprot_swissprot" ]; then
    wget ftp://ftp.uniprot.org/pub/databases/uniprot/current_release/knowledgebase/complete/uniprot_sprot.fasta.gz
    FNAME=uniprot_sprot.fasta.gz
    DBNAME=uniprot_sprot.fastaDB
elif [ "$DLNAME" -eq "uniprot_trembl" ]; then
    wget ftp://ftp.uniprot.org/pub/databases/uniprot/current_release/knowledgebase/complete/uniprot_trembl.fasta.gz
    FNAME=uniprot_trembl.fasta.gz
    DBNAME=uniprot_trembl.fastaDB
else
    echo "Meh"
    exit 1;
fi

CREATEDB=1
if [ "$CREATEDB" -eq "1" ]; then
    "$MMSEQS" createdb "${FNAME}" "${DBNAME}" "${DBNAME}.tmp" \
        || fail "query createdb died"
fi

CREATEINDEX=0
if [ "$CREATEINDEX" -eq "1" ]; then
    "$MMSEQS" createindex "${DBNAME}" "${DBNAME}.tmp" \
        || fail "query createdb died"
fi


ADDTAXINFO=1
if [ "$ADDTAXINFO" -eq "1" ]; then
    "$MMSEQS" createtaxdb "${DBNAME}" "${DBNAME}.tmp" \
        || fail "query createtaxdb died"
fi


