/*-------------------------------------------------------------------------
 *
 * rewriteSupport.c
 *
 *
 * Portions Copyright (c) 1996-2000, PostgreSQL, Inc
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $Header$
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/heapam.h"
#include "catalog/catname.h"
#include "catalog/indexing.h"
#include "rewrite/rewriteSupport.h"
#include "utils/catcache.h"
#include "utils/syscache.h"


int
IsDefinedRewriteRule(char *ruleName)
{
	HeapTuple	tuple;

	tuple = SearchSysCacheTuple(RULENAME,
								PointerGetDatum(ruleName),
								0, 0, 0);
	return HeapTupleIsValid(tuple);
}

/*
 * SetRelationRuleStatus
 *		Set the value of the relation's relhasrules field in pg_class;
 *		if the relation is becoming a view, also adjust its relkind.
 *
 * NOTE: caller must be holding an appropriate lock on the relation.
 *
 * NOTE: an important side-effect of this operation is that an SI invalidation
 * message is sent out to all backends --- including me --- causing relcache
 * entries to be flushed or updated with the new set of rules for the table.
 * Therefore, we execute the update even if relhasrules has the right value
 * already.  Possible future improvement: skip the disk update and just send
 * an SI message in that case.
 */
void
SetRelationRuleStatus(Oid relationId, bool relHasRules,
					  bool relIsBecomingView)
{
	Relation	relationRelation;
	HeapTuple	tuple;
	Relation	idescs[Num_pg_class_indices];

	/*
	 * Find the tuple to update in pg_class, using syscache for the lookup.
	 */
	relationRelation = heap_openr(RelationRelationName, RowExclusiveLock);
	tuple = SearchSysCacheTupleCopy(RELOID,
									ObjectIdGetDatum(relationId),
									0, 0, 0);
	Assert(HeapTupleIsValid(tuple));

	/* Do the update */
	((Form_pg_class) GETSTRUCT(tuple))->relhasrules = relHasRules;
	if (relIsBecomingView)
		((Form_pg_class) GETSTRUCT(tuple))->relkind = RELKIND_VIEW;

	heap_update(relationRelation, &tuple->t_self, tuple, NULL);

	/* Keep the catalog indices up to date */
	CatalogOpenIndices(Num_pg_class_indices, Name_pg_class_indices, idescs);
	CatalogIndexInsert(idescs, Num_pg_class_indices, relationRelation, tuple);
	CatalogCloseIndices(Num_pg_class_indices, idescs);

	heap_freetuple(tuple);
	heap_close(relationRelation, RowExclusiveLock);
}
