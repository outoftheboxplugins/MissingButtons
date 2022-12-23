﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "QuickMenuDiscoverySubsystem.h"


#define LOCTEXT_NAMESPACE "LevelEditorMenu"

FQuickCommandEntry::FQuickCommandEntry(const TSharedRef<FUICommandInfo>& Command, const TSharedRef<FUICommandList> CommandList)
{
	Icon = Command->GetIcon();

	Title = Command->GetLabel();
	Tooltip = Command->GetDescription();

	const FUIAction* UIAction = CommandList->GetActionForCommand(Command);
	if (ensure(UIAction))
	{
		ExecuteCallback = UIAction->ExecuteAction;
		CanExecuteCallback = UIAction->CanExecuteAction;
	}
}

TArray<FQuickCommandEntry> UQuickMenuDiscoverySubsystem::GetAllCommands() const
{
	TArray<FQuickCommandEntry> Result;

	GatherCommandsInternal(Result);
	OnDiscoverCommands.Broadcast(Result);

	return Result;
}

void UQuickMenuDiscoverySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UQuickMenuDiscoverySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UQuickMenuDiscoverySubsystem::GatherCommandsInternal(TArray<FQuickCommandEntry>& OutCommands) const
{
	TArray<UQuickMenuExtension*> Extensions;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* CurrentClass = (*It);

		if (CurrentClass->IsChildOf(UQuickMenuExtension::StaticClass()) && !(CurrentClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UQuickMenuExtension* QuickMenuExtension = Cast<UQuickMenuExtension>(CurrentClass->GetDefaultObject());
			if (QuickMenuExtension->ShouldShow())
			{
				Extensions.Add(QuickMenuExtension);
			}
		}
	}

	Extensions.Sort(
		[](const UQuickMenuExtension& A, const UQuickMenuExtension& B)
		{
			return A.GetPriority() > B.GetPriority();
		}
	);

	for (UQuickMenuExtension* Extension : Extensions)
	{
		OutCommands.Append(Extension->GetCommands());
	}
	PopulateMenuEntries(OutCommands);
}

void UQuickMenuDiscoverySubsystem::PopulateMenuEntries(TArray<FQuickCommandEntry>& OutCommands) const
{
}

#undef LOCTEXT_NAMESPACE